/*
ide_dma.cpp

Copyright (c) 23 Yann BOUCHER (yann)

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

#include "ide_dma.hpp"

#include "mem/memmap.hpp"
#include "io.hpp"
#include "utils/bitops.hpp"
#include "utils/memutils.hpp"
#include "time/timer.hpp"
#include "i686/interrupts/interrupts.hpp"

#include "tasking/process.hpp"
#include "tasking/scheduler.hpp"

#include "ide_common.hpp"

#include "drivers/sound/beep.hpp"

#include "utils/nop.hpp"

namespace ide::dma
{

struct PRD
{
    uint32_t phys_buf_addr;
    uint16_t byte_count;
    uint16_t reserved : 15;
    uint16_t end_of_prdt : 1;
};
static_assert(sizeof(PRD) == 8);

alignas(0x10000) static PRD PRDT[0x10000 / sizeof(PRD)];

volatile bool primary_master_int { false };
volatile bool primary_slave_int { false };
volatile bool secondary_master_int { false };
volatile bool secondary_slave_int { false };

volatile bool raised_ints[2][2] { {false, false}, {false, false} };
Process*      waiting_processes[2][2];
int drive_status[2][2];
constexpr int status_ok = -1;

bool Controller::accept(const pci::PciDevice &dev)
{
    if (dev.classCode != 0x1 || dev.subclass != 0x1)
        return false;

    bool can_bus_master = dev.progIF & 0x80;
    if (can_bus_master)
        return true;
    else
        return false;
}

void Controller::init()
{
    enable_io_space();
    enable_bus_mastering();
    auto reg = pci::read16(m_dev.bus, m_dev.slot, m_dev.func, pci::Reg::Command);
    reg &= 0xfbff; // enable interrupts
    pci::write16(m_dev.bus, m_dev.slot, m_dev.func, pci::Reg::Command, reg);

    m_dev = pci::get_dev(m_dev.bus, m_dev.slot, m_dev.func); // re-read info

    assert(pci::bar_type(m_dev.bar[4]) == pci::BARType::IO16);

    log(Debug, "ATA DMA PCI bus mastering enabled on PCI address 0x%x:0x%x:0x%x\n", m_dev.bus, m_dev.slot, m_dev.func);
    log(Debug, "Programming interface bits : %08b (cmd : %08b)\n", m_dev.progIF, m_dev.command);

    m_primary_compatibility = !(m_dev.progIF & (1<<0));
    m_primary_compatibility = !(m_dev.progIF & (1<<2));

    if (m_primary_compatibility)
        isr::register_handler(IRQ14, [this](const registers* r){return int14_handler(r);});
    if (m_secondary_compatibility)
        isr::register_handler(IRQ15, [this](const registers* r){return int15_handler(r);});

    log(Debug, "PCI Interrupt : %d\n", m_dev.int_line);

    if ((!m_primary_compatibility || !m_secondary_compatibility) && m_dev.int_line != 0)
    {
        isr::register_handler(IRQ0 + m_dev.int_line, [](const registers*)
        {
            log(Debug, "Booh it was called !\n");
            return true;
        });
    }

    for (auto pair : scan())
    {
        ide::dma::Disk::create_disk(*this, (BusPort)pair.first, (DriveType)pair.second);
        log(Debug, "Created ATA DMA disk on 0x%x 0x%x\n", io_base((BusPort)pair.first), pair.second);
    }

    memset((void*)raised_ints, 0, sizeof(raised_ints));
}

bool Controller::common_handler(const ata_device& dev)
{
    auto status = status_byte(dev.port);

    if (status & (1<<0))
    {
        log_serial("Received PCI ATA IRQ on port 0x%x, status 0x%x\n", dev.port, status);
    }

    if (status & (1<<2))
    {
        bool slave = drive_register(dev) & (1<<4);

        if ((status & (1<<0)) == 0 || true) // TODO : investigate why sometimes it hangs when there are PRD pending
        {
            auto* process = waiting_processes[dev.port==BusPort::Primary][slave];
            raised_ints[dev.port==BusPort::Primary][slave] = true;

            if (process)
            {
                process->status = Process::Active;
            }

            send_command_byte(dev.port, 0); // clear start/stop bit

            if (status & (1<<1))
            {
                DiskError::Type error = get_error(dev);
                drive_status[dev.port==BusPort::Primary][slave] = error;
                warn("IDE DMA Error on port 0x%x, error 0x%x\n", dev.port, error);
            }
            else
            {
                drive_status[dev.port==BusPort::Primary][slave] = status_ok;
            }

        }
        bit_clear(status, 0); bit_clear(status, 1); // clear error and interrupt bits
        send_status_byte(dev.port, status);
    }

    return true;
}

bool Controller::int14_handler(const registers *)
{
    return common_handler(mk_dev(BusPort::Primary, (DriveType)0));
}

bool Controller::int15_handler(const registers *)
{
    return common_handler(mk_dev(BusPort::Secondary, (DriveType)0));
}

std::vector<std::pair<uint16_t, uint8_t> > Controller::scan()
{
    std::vector<std::pair<uint16_t, uint8_t>> result;

    for (auto bus : {Primary, Secondary})
    {
        for (auto type : {Slave, Master})
        {
            auto dev = mk_dev(bus, type);
            if (detect(dev) && identify(dev))
            {
                result.emplace_back(bus, type);
            }
        }
    }

    return result;
}

void Controller::send_command(const ata_device &dev, uint8_t command, bool read, size_t block, size_t count, gsl::span<const uint8_t> data)
{
    auto status = status_byte(dev.port);
    bit_clear(status, 0); bit_clear(status, 1); // clear error and interrupt bits
    send_status_byte(dev.port, status);

    send_command_byte(dev.port, (!(read)&1) << 3); // set operation direction

    prepare_prdt(dev.port, data);

    select(dev, block, count);

    outb(dev.io_base + 7, command);

    //assert(interrupts_enabled());
    sti();

    send_command_byte(dev.port, ((read&1) << 3) | 0b1); // set start bit
}

uint16_t Controller::io_base(BusPort bus)
{
    if (bus == BusPort::Primary)
    {
        if (m_primary_compatibility || m_dev.bar[0] == 0 || m_dev.bar[0] == 1)
            return 0x1F0;

        assert(pci::bar_type(m_dev.bar[0]) == pci::BARType::IO16);

        return pci::get_bar_val(m_dev, 0);
    }
    else
    {
        if (m_secondary_compatibility || m_dev.bar[2] == 0 || m_dev.bar[2] == 1)
            return 0x170;

        assert(pci::bar_type(m_dev.bar[2]) == pci::BARType::IO16);

        return pci::get_bar_val(m_dev, 2);
    }
}

uint16_t Controller::control_io_base(BusPort bus)
{
    if (bus == BusPort::Primary)
    {
        if (m_primary_compatibility || m_dev.bar[1] == 0 || m_dev.bar[1] == 1)
            return 0x3F6;

        assert(pci::bar_type(m_dev.bar[1]) == pci::BARType::IO16);

        return pci::get_bar_val(m_dev, 1);
    }
    else
    {
        if (m_secondary_compatibility || m_dev.bar[3] == 0 || m_dev.bar[3] == 1)
            return 0x376;

        assert(pci::bar_type(m_dev.bar[3]) == pci::BARType::IO16);

        return pci::get_bar_val(m_dev, 3);
    }
}

void Controller::prepare_prdt(BusPort bus, gsl::span<const uint8_t> data)
{
    PRD* prd_ptr = (PRD*)PRDT;

    uintptr_t begin = (uintptr_t)data.data();
    uintptr_t end = begin + data.size();

    for (size_t pg = begin; pg < end; pg += Memory::page_size())
    {
        size_t addr = Memory::physical_address((void*)pg);
        size_t size = Memory::page(pg) == Memory::page(end) ? Memory::offset(end) - Memory::offset(pg) : Memory::page_size() - Memory::offset(addr);

        pg = Memory::page(pg);

        assert(size < 4*1024*1024);

        prd_ptr->byte_count = size;
        prd_ptr->phys_buf_addr = addr;
        prd_ptr->end_of_prdt = (pg == Memory::page(end));

        ++prd_ptr;
    }

    send_prdt(bus);
}

uint8_t Controller::status_byte(BusPort bus)
{
    uint16_t port = pci::get_bar_val(m_dev, 4) + (bus==BusPort::Primary?0x2:0xA);

    return inb(port);
}

void Controller::send_status_byte(BusPort bus, uint8_t val)
{
    uint16_t port = pci::get_bar_val(m_dev, 4) + (bus==BusPort::Primary?0x2:0xA);

    outb(port, val);
}

void Controller::send_command_byte(BusPort bus, uint8_t val)
{
    uint16_t port = pci::get_bar_val(m_dev, 4) + (bus==BusPort::Primary?0x0:0x8);

    outb(port, val);
}

void Controller::send_prdt(BusPort bus)
{
    uint16_t port = pci::get_bar_val(m_dev, 4) + (bus==BusPort::Primary?0x4:0xC);

    outl(port, Memory::physical_address(PRDT));
}

Disk::Disk(Controller& controller, BusPort port, DriveType type)
    : IDEDisk(controller.mk_dev(port, type)), m_cont(controller)
{

}

[[nodiscard]]
kpp::expected<kpp::dummy_t, DiskError> Disk::do_read_write(size_t sector, gsl::span<const uint8_t> data, RWAction action) const
{
    const size_t count = data.size() / sector_size() + (data.size()%sector_size()?1:0);

    volatile auto& int_status = raised_ints[m_dev.port==BusPort::Primary][m_dev.type==DriveType::Slave];

    waiting_processes[m_dev.port==BusPort::Primary][m_dev.type==DriveType::Slave] = &Process::current();

    int_status = false;

    (void)ide::status_register(m_dev); // read status port to reset drive

    Process::current().status = Process::IOWait;
    m_cont.send_command(m_dev, ata_read_dma_ex, action == RWAction::Read,
                        sector, count, data);

#if 1
    if (!int_status)
    {
        tasking::kernel_yield();
    }
#else
    if (!Timer::sleep_until([&int_status]{return int_status;}, 1000))
    {
        return kpp::make_unexpected(DiskError{DiskError::TimeOut});
    }
#endif

    int_status = false;

    auto status = drive_status[m_dev.port==BusPort::Primary][m_dev.type==DriveType::Slave];

    if (status != status_ok)
    {
        return kpp::make_unexpected(DiskError{(DiskError::Type)status});
    }

    return {};
}

[[nodiscard]]
kpp::expected<kpp::dummy_t, DiskError> Disk::read_sectors(size_t sector, gsl::span<uint8_t> data) const
{
    if (auto result = do_read_write(sector, data, RWAction::Read); !result)
        return kpp::make_unexpected(result.error());

    return {};
}

[[nodiscard]]
kpp::expected<kpp::dummy_t, DiskError> Disk::write_sectors(size_t sector, gsl::span<const uint8_t> data)
{
    assert(data.size() % sector_size() == 0);
    assert(sector <= m_id_data->sectors_48);

    if (auto result = do_read_write(sector, data, RWAction::Write); !result)
        return kpp::make_unexpected(result.error());

    return {};
}

ADD_PCI_DRIVER(Controller);

}
