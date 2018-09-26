/*
ahci.cpp

Copyright (c) 07 Yann BOUCHER (yann)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "ahci.hpp"

#include "drivers/pci/pci.hpp"

#include "utils/logging.hpp"
#include "utils/bitops.hpp"
#include "utils/nop.hpp"
#include "time/timer.hpp"
#include "mem/memmap.hpp"

#include "i686/interrupts/isr.hpp"


namespace ahci
{

volatile detail::HBAMem* volatile mem;

alignas(1024) detail::CommandList cmdlists[32];
alignas(128)  detail::CommandTable cmdtables[32];
alignas(256)  detail::ReceivedFIS rcvfis[32];

bool available()
{
    return !pci::find_devices(0x1, 0x6, 0x1).empty();
}


bool init()
{
    if (!available()) return false;

    mem = detail::get_hbamem_ptr();
    if (!mem)
    {
        log(Debug, "No valid AHCI controller found\n");
        return false;
    }

    mem->ghc |= detail::ghd_ahci_enable;

    detail::get_ahci_ownership();

    log(Info, "AHCI version : %d%d.%d%d\n", (mem->major_vs>>8)&0xFF, mem->major_vs&0xFF, (mem->minor_vs>>8)&0xFF, mem->minor_vs&0xFF);
    log(Info, "AHCI capabilities : %b\n", mem->cap);

    log(Info, "Available AHCI ports : \n");

    size_t port_count { 0 };

    for (size_t i { 0 }; i < sizeof(mem->pi)*CHAR_BIT; ++i)
    {
        if (bit_check(mem->pi, i))
        {
            detail::init_port(i);
            auto type = detail::get_port_type(i);
            if (type == detail::PortType::SATA) ++port_count;
            log(Info, "   Port %u, type %s\n", i, type == detail::PortType::SATA ? "SATA" :
                                                                                   type == detail::PortType::SATAPI ? "SATAPI" :
                                                                                                                      type == detail::PortType::SEMB ? "SEMB" :
                                                                                                                                                       type == detail::PortType::PM ? "PM" : "Null");
        }
    }

    if (port_count == 0)
    {
        warn("AHCI present but no ports available, disabling\n");
        return false;
    }

    log(Debug, "AHCI interrupt line : %d\n", detail::get_interrupt_line());

    isr::register_handler(detail::get_interrupt_line(), &detail::ahci_isr);

    //mem->ghc |= detail::ghd_int_enable;

    detail::init_interface();

    return true;
}

size_t ahci::Disk::disk_size() const
{
    if (!m_id_data) update_id_data();

    if (m_id_data) return m_id_data->sectors_48*512;
    else return 0;
}

size_t ahci::Disk::sector_size() const
{
    if (!m_id_data) update_id_data();

    if (m_id_data) return m_id_data->sector_size*2?:512;
    else return 512;
}

kpp::string ahci::Disk::drive_name() const
{
    if (!m_id_data) update_id_data();

    if (m_id_data) return ide::ata_string(m_id_data->model);
    else return "<invalid>";
}

void Disk::flush_hardware_cache()
{
    (void)detail::issue_cache_flush_command(m_port);
}

ahci::Disk::Disk(uint16_t port)
    : DiskImpl<ahci::Disk>()
{
    m_port = port;
}

void ahci::Disk::update_id_data() const
{
    ide::identify_data data;
    if (detail::issue_identify_command(m_port, &data))
    {
        m_id_data = data;
    }

    if (!m_id_data)
    {
        warn("AHCI port %d returned invalid identify data\n", m_port);
    }
}

[[nodiscard]]
kpp::expected<MemBuffer, DiskError> ahci::Disk::read_sector(size_t sector, size_t count) const
{
    MemBuffer data(count * sector_size());
    if (detail::do_read(m_port, sector, count, (uint16_t*)data.data()))
    {
        return std::move(data);
    }
    else
    {
        return kpp::make_unexpected(DiskError{DiskError::Unknown});
    }
}

[[nodiscard]]
kpp::expected<kpp::dummy_t, DiskError> ahci::Disk::write_sector(size_t sector, gsl::span<const uint8_t> data)
{
    const size_t count = data.size() / sector_size() + (data.size()%sector_size()?1:0);

    if (!detail::issue_write_command(m_port, sector, count, (const uint16_t*)data.data()))
    {
        return kpp::make_unexpected(DiskError{DiskError::Unknown});
    }

    return {};
}

uint8_t detail::get_interrupt_line()
{
    auto ahci_con = pci::find_devices(0x1, 0x6, 0x1)[0];

    return ahci_con.int_line;
}

detail::HBAMem *detail::get_hbamem_ptr()
{
    if (!available()) return nullptr;

    auto ahci_con = pci::find_devices(0x1, 0x6, 0x1)[0];

    auto bar = pci::get_bar_val(ahci_con, 5);

    return reinterpret_cast<HBAMem*>(Memory::mmap(bar, 0x1100, Memory::Read|Memory::Write|Memory::Uncached));
}

void detail::get_ahci_ownership()
{
    mem->bohc |= bohc_os_ownership;

    while (mem->bohc & bohc_bios_ownership) { nop(); };

    Timer::sleep(25); // allow 25ms in the spec

    Timer::sleep_until([&]{return !(mem->bohc & bohc_bios_busy);}, 2000); // allow the BIOS to end any operation; the spec allows for 2sec
}

bool detail::ahci_isr(const registers *)
{
    log(Debug, "Interrupt !!\n");
    return true;
}

uint32_t detail::flush_commands(size_t port)
{
    /* the commands may not take effect until the command
     * register is read again by software, because reasons.
     */
    volatile uint32_t c = mem->ports[port].ci;
    c=c;
    return c;
}

void detail::mkprd(PrdtEntry& entry, uint64_t addr, size_t bytes)
{
    assert(bytes <= 4*1024*1024);

    entry.dba = addr & 0xFFFFFFFF;
    if (mem->s64a)
    {
        entry.dbau = (addr >> 32) & 0xFFFFFFFF;
    }

    entry.dbc = bytes - 1; // zero indexed
    entry.i = 1;
}

bool detail::issue_read_command(size_t port, uint64_t sector, size_t count, uint16_t* buf)
{
    int slot = free_slot(port);
    if (slot < 0)
    {
        warn("No more AHCI slots available\n");
        return false;
    }

    //CommandHeader *cmdheader = reinterpret_cast<CommandHeader*>(mem->ports[port].clb);
    CommandHeader* cmdheader = &(cmdlists[port].hdrs[slot]);

    cmdheader->cfl = sizeof(FisRegH2D)/sizeof(uint32_t);	// Command FIS size
    cmdheader->write = 0;		// Read from device
    cmdheader->prdtl = ((count-1)>>4) + 1;	// PRDT entries count

    cmdheader->atapi = false;

    //CommandTable *cmdtbl = reinterpret_cast<CommandTable*>(cmdheader->ctba);
    CommandTable* cmdtbl = &cmdtables[port];
    memset(cmdtbl, 0, sizeof(CommandTable) +
           (cmdheader->prdtl-1)*sizeof(PrdtEntry));
    // 8K bytes (16 sectors) per PRDT

    int i;
    for (i=0; i<cmdheader->prdtl-1; i++)
    {
        mkprd(cmdtbl->entries[i], Memory::physical_address(buf), 4*1024*1024);
        buf += 4*1024*1024;
        count -= 8;	// 16 sectors
    }
    mkprd(cmdtbl->entries[i], Memory::physical_address(buf), count*512);

    // Setup command
    FisRegH2D *cmdfis = reinterpret_cast<FisRegH2D*>(&cmdtbl->command_fis);
    //FisRegH2D *cmdfis = (FisRegH2D*)&rcvfis[port];

    cmdfis->fis_type = FISType::RegH2D;
    cmdfis->c = 1;	// Command
    cmdfis->command = ata_read_dma_ex;

    cmdfis->lba0 = sector&0xFF;
    cmdfis->lba1 = (sector>>8)&0xFF;
    cmdfis->lba2 = (sector>>16)&0xFF;
    cmdfis->device = 1<<6;	// LBA mode

    cmdfis->lba3 = (sector>>24)&0xFF;
    cmdfis->lba4 = (sector>>32)&0xFF;
    cmdfis->lba5 = (sector>>40)&0xFF;

    cmdfis->countl = count&0xFF;
    cmdfis->counth = (count>>8)&0xFF;

    if (!Timer::sleep_until([&]{return !(mem->ports[port].tfd & (ata_busy | ata_drq));}, 500))
    {
        warn("AHCI port %d is hung\n", port);
        return false;
    }

    mem->ports[port].is = ~0; // clear interrupt flags
    mem->ports[port].ci = 1<<slot;	// Issue command
    flush_commands(port);

    // Wait for completion
    while (true)
    {
        // In some longer duration reads, it may be helpful to spin on the DPS bit
        // in the PxIS port field as well (1 << 5)
        if (((mem->ports[port].sact | mem->ports[port].ci) & (1<<slot)) == 0 &&
                (mem->ports[port].is & pxis_dps) == 0)
            break;
        if (check_errors(port))	// Task file error
        {
            err("Read disk error on AHCI port %d\n", port);
            return false;
        }
    }

    // Check again
    if (check_errors(port))
    {
        err("::Read disk error on AHCI port %d\n", port);
        return false;
    }

    return true;
}

bool detail::issue_write_command(size_t port, uint64_t sector, size_t count, const uint16_t* buf)
{
    int slot = free_slot(port);
    if (slot < 0)
    {
        warn("No more AHCI slots available\n");
        return false;
    }

    CommandHeader *cmdheader = &(cmdlists[port].hdrs[slot]);
    cmdheader += slot;

    cmdheader->cfl = sizeof(FisRegH2D)/sizeof(uint32_t);	// Command FIS size
    cmdheader->write = 1;		// Write to device
    cmdheader->prdtl = ((count-1)>>4) + 1;	// PRDT entries count
    cmdheader->atapi = false;

    //CommandTable *cmdtbl = reinterpret_cast<CommandTable*>(cmdheader->ctba);
    CommandTable *cmdtbl = &cmdtables[port];
    memset(cmdtbl, 0, sizeof(CommandTable) +
           (cmdheader->prdtl-1)*sizeof(PrdtEntry));

    int i;
    for (i=0; i<cmdheader->prdtl-1; i++)
    {
        mkprd(cmdtbl->entries[i], Memory::physical_address(buf), 4*1024*1024);
        buf += 4*1024*1024;
        count -= 8;	// 16 sectors
    }

    mkprd(cmdtbl->entries[i], Memory::physical_address(buf), count*512);

    // Setup command
    FisRegH2D *cmdfis = reinterpret_cast<FisRegH2D*>(&cmdtbl->command_fis);

    cmdfis->fis_type = FISType::RegH2D;
    cmdfis->c = 1;	// Command
    cmdfis->command = ata_write_dma_ex;

    cmdfis->lba0 = sector&0xFF;
    cmdfis->lba1 = (sector>>8)&0xFF;
    cmdfis->lba2 = (sector>>16)&0xFF;
    cmdfis->device = 1<<6;	// LBA mode

    cmdfis->lba3 = (sector>>24)&0xFF;
    cmdfis->lba4 = (sector>>32)&0xFF;
    cmdfis->lba5 = (sector>>40)&0xFF;

    cmdfis->countl = count&0xFF;
    cmdfis->counth = (count>>8)&0xFF;

    if (!Timer::sleep_until([&]{return !(mem->ports[port].tfd & (ata_busy | ata_drq));}, 500))
    {
        warn("AHCI port %d is hung\n", port);
        return false;
    }

    mem->ports[port].is = ~0; // clear interrupt flags
    mem->ports[port].ci = 1<<slot;	// Issue command
    flush_commands(port);
    // Wait for completion
    while (true)
    {
        // In some longer duration reads, it may be helpful to spin on the DPS bit
        // in the PxIS port field as well (1 << 5)
        if (((mem->ports[port].sact | mem->ports[port].ci) & (1<<slot)) == 0)
            break;
        if (mem->ports[port].is & pxis_tfes)	// Task file error
        {
            warn("Write disk error on AHCI port %d\n", port);
            return false;
        }
    }

    // Check again
    if (mem->ports[port].is & pxis_tfes)
    {
        warn("Write disk error on AHCI port %d\n", port);
        return false;
    }

    return true;
}

bool detail::issue_identify_command(size_t port, ide::identify_data* buf)
{
    int slot = free_slot(port);
    if (slot < 0)
    {
        warn("No more AHCI slots available\n");
        return false;
    }

    CommandHeader* cmdheader = &(cmdlists[port].hdrs[slot]);

    cmdheader->cfl = sizeof(FisRegH2D)/sizeof(uint32_t);	// Command FIS size
    cmdheader->write = 0;		// Read from device
    cmdheader->prdtl = 1;	// PRDT entries count

    cmdheader->atapi = false;

    CommandTable* cmdtbl = &cmdtables[port];
    memset(cmdtbl, 0, sizeof(CommandTable) +
           (cmdheader->prdtl-1)*sizeof(PrdtEntry));
    // 8K bytes (16 sectors) per PRDT

    mkprd(cmdtbl->entries[0], Memory::physical_address(buf), 512);

    // Setup command
    FisRegH2D *cmdfis = reinterpret_cast<FisRegH2D*>(&cmdtbl->command_fis);

    cmdfis->fis_type = FISType::RegH2D;
    cmdfis->c = 1;	// Command
    cmdfis->command = ata_identify;

    if (!Timer::sleep_until([&]{return !(mem->ports[port].tfd & (ata_busy | ata_drq));}, 500))
    {
        warn("AHCI port %d is hung\n", port);
        return false;
    }

    mem->ports[port].is = ~0; // clear interrupt flags
    mem->ports[port].ci = 1<<slot;	// Issue command
    flush_commands(port);

    // Wait for completion
    while (true)
    {
        // In some longer duration reads, it may be helpful to spin on the DPS bit
        // in the PxIS port field as well (1 << 5)
        if (((mem->ports[port].sact | mem->ports[port].ci) & (1<<slot)) == 0 &&
                (mem->ports[port].is & pxis_dps) == 0)
            break;
        if (check_errors(port))	// Task file error
        {
            err("Read disk error on AHCI port %d\n", port);
            return false;
        }
    }

    // Check again
    if (check_errors(port))
    {
        err("::Read disk error on AHCI port %d\n", port);
        return false;
    }

    return true;
}

bool detail::issue_cache_flush_command(size_t port)
{
    int slot = free_slot(port);
    if (slot < 0)
    {
        warn("No more AHCI slots available\n");
        return false;
    }

    CommandHeader* cmdheader = &(cmdlists[port].hdrs[slot]);

    cmdheader->cfl = sizeof(FisRegH2D)/sizeof(uint32_t);	// Command FIS size
    cmdheader->write = 1;
    cmdheader->prdtl = 0;	// PRDT entries count

    cmdheader->atapi = false;

    CommandTable* cmdtbl = &cmdtables[port];
    memset(cmdtbl, 0, sizeof(CommandTable) +
           (cmdheader->prdtl-1)*sizeof(PrdtEntry));

    // Setup command
    FisRegH2D *cmdfis = reinterpret_cast<FisRegH2D*>(&cmdtbl->command_fis);

    cmdfis->fis_type = FISType::RegH2D;
    cmdfis->c = 1;	// Command
    cmdfis->command = ata_flush_ext;

    if (!Timer::sleep_until([&]{return !(mem->ports[port].tfd & (ata_busy | ata_drq));}, 500))
    {
        warn("AHCI port %d is hung\n", port);
        return false;
    }

    mem->ports[port].is = ~0; // clear interrupt flags
    mem->ports[port].ci = 1<<slot;	// Issue command
    flush_commands(port);

    // Wait for completion
    while (true)
    {
        // In some longer duration reads, it may be helpful to spin on the DPS bit
        // in the PxIS port field as well (1 << 5)
        if (((mem->ports[port].sact | mem->ports[port].ci) & (1<<slot)) == 0 &&
                (mem->ports[port].is & pxis_dps) == 0)
            break;
        if (check_errors(port))	// Task file error
        {
            err("Read disk error on AHCI port %d\n", port);
            return false;
        }
    }

    // Check again
    if (check_errors(port))
    {
        err("::Read disk error on AHCI port %d\n", port);
        return false;
    }

    return true;
}

void detail::init_interface()
{
    for (size_t port { 0 }; port < sizeof(mem->pi)*CHAR_BIT; ++port)
    {
        if (bit_check(mem->pi, port) && detail::get_port_type(port) == PortType::SATA)
        {
            ahci::Disk::create_disk(port);
        }
    }
}

bool detail::do_read(size_t port, uint64_t sector, size_t count, uint16_t *buf)
{
#if 0
    /*static*/ uint16_t temp[256];
    for (size_t i { 0 }; i < count; ++i)
    {
        if (!issue_read_command(port, sector + i, 1, temp))
        {
            return false;
        }

        memcpy(buf, temp, 512);
        buf += 256;
    }

    return true;
#else
    return issue_read_command(port, sector, count, buf);
#endif
}

}
