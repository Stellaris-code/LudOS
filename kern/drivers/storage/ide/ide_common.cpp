/*
ide_common.cpp

Copyright (c) 29 Yann BOUCHER (yann)

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

#include "ide_common.hpp"

#include "io.hpp"

#include <array.hpp>

#include "utils/logging.hpp"
#include "utils/bitops.hpp"
#include "utils/nop.hpp"

namespace ide
{

const char* to_string(BusPort bus_port)
{
    if (bus_port == Primary)
    {
        return "Primary";
    }
    else if (bus_port == Secondary)
    {
        return "Secondary";
    }
    else if (bus_port == Third)
    {
        return "Third";
    }
    else
    {
        return "Fourth";
    }
}

void wait_400ns(const ata_device& dev)
{
    for (size_t i { 0 }; i < 4; ++i)
    {
        inb(dev.control_base + ATA_ALT_STATUS);
    }
}

void select(const ata_device& dev, uint64_t block, uint16_t count)
{
    outb(dev.io_base + ATA_SELECT, 0x40 | dev.type);

    outb(dev.io_base + ATA_SECCOUNT, (count >> 8) & 0xFF);

    outb(dev.io_base + ATA_LBALO, (block >> 24) & 0xFF);
    outb(dev.io_base + ATA_LBAMID, (block >> 32) & 0xFF);
    outb(dev.io_base + ATA_LBAHI, (block >> 40) & 0xFF);

    outb(dev.io_base + ATA_SECCOUNT, count & 0xFF);

    outb(dev.io_base + ATA_LBALO, block & 0xFF);
    outb(dev.io_base + ATA_LBAMID, (block >> 8) & 0xFF);
    outb(dev.io_base + ATA_LBAHI, (block >> 16) & 0xFF);

    wait_400ns(dev);
}

kpp::optional<identify_data> identify(const ata_device& dev)
{
    if (inb(dev.io_base + 7) == 0xFF)
    {
        log(Notice, "Tried to identify an inexistent io port\n");
        return {};
    }

    const char* port_str = to_string(dev.port);
    const char* type_str = dev.type==Master?"master":"slave";

    poll_bsy(dev);

    outb(dev.io_base + ATA_FEATURES, 0);
    outb(dev.control_base + ATA_DEVCTRL, 0);

    outb(dev.io_base + ATA_SELECT, (dev.type == Master ? 0xA0 : 0xB0)); // select

    wait_400ns(dev);

    poll_bsy(dev);
    if (error_set(dev))
    {
        log(Debug, "Error set on select\n");
        return {};
    }

    outb(dev.io_base + ATA_CMD, ata_identify); // TODO : send_cmd
    uint8_t status = inb(dev.io_base + ATA_STATUS);

    if (status == 0)
    {
        log(Debug, "ATA %s %s doesn't exist.\n", port_str, type_str);
        return {};
    }

    status = poll_bsy(dev);

    size_t max_iter = 0x10000;
    while(!(status & ATA_DRQ) && max_iter-- > 0)
    {
        status = inb(dev.io_base + ATA_STATUS);
        if(status & ATA_ERR)
        {
            log(Debug, "ATA %s %s has ERR set. Disabled.\n", port_str, type_str);
            return {};
        }
    }
    if (max_iter == 0)
    {
        log(Debug, "Timed out\n");
        return {};
    }

    log(Info, "ATA %s %s is online.\n", port_str, type_str);

    kpp::array<uint16_t, 256> buffer;

#if 1
    for(size_t i = 0; i<256; i++)
    {
        buffer[i] = inw(dev.io_base + ATA_DATA);
    }
#else
    insw(dev.io_base + 0, buffer.data(), 256);
#endif

    identify_data* id_data = (identify_data*)buffer.data();

    if ((id_data->flags & (1<<15)) != 0)
    {
        log(Notice, "Invalid identify data on ATA %s %s\n", port_str, type_str);
        return {};
    }

    if (id_data->sector_size != 0 || id_data->sectors_28 == 0xFF || id_data->sectors_48 == 0xFF)
    {
        log(Notice, "Identify data looks invalid on ATA %s %s\n", port_str, type_str);
        return {};
    }

    log(Info, "Firmware : %s, model : %s\n", ata_string(id_data->firmware).c_str(), ata_string(id_data->model).c_str());
    log(Info, "Sector size : %d, sectors (48) : %d / sectors (28) : %d\n", id_data->sector_size, id_data->sectors_48, id_data->sectors_28);

    return *id_data;
}

uint8_t poll(const ata_device& dev)
{
    size_t max_iters { 0x10000 };
    uint8_t status;
    while (!(status = (inb(dev.control_base + ATA_ALT_STATUS)) & ATA_DRQ) && --max_iters > 0) { nop(); }
    return status;
}

uint8_t poll_bsy(const ata_device& dev)
{
    size_t max_iters { 0x10000 };
    uint8_t status;
    while ((status = (inb(dev.control_base + ATA_ALT_STATUS)) & ATA_BSY) && --max_iters > 0) { nop(); }
    return status;
}

bool flush(const ata_device& dev)
{
    poll_bsy(dev);
    poll(dev);

    outb(dev.io_base + ATA_CMD, ata_flush_ext);

    poll_bsy(dev);
    poll(dev);

    if (error_set(dev))
    {
        bool stat = !error_set(dev);
        if (!stat)
        {
            clear_error(dev);

            return stat;
        }
    }

    return true;
}

bool error_set(const ata_device& dev)
{
    const uint8_t status = status_register(dev);
    return bit_check(status, 0) || bit_check(status, 5);
}

void clear_error(const ata_device& dev)
{
    outb(dev.io_base + ATA_CMD, ata_nop);
}

uint8_t error_register(const ata_device& dev)
{
    return inb(dev.io_base + ATA_ERR);
}

uint8_t status_register(const ata_device& dev)
{
    return inb(dev.io_base + ATA_STATUS);
}

uint8_t drive_register(const ata_device& dev)
{
    return inb(dev.io_base + ATA_SELECT);
}

DiskError::Type get_error(const ata_device& dev)
{
    auto err = inb(dev.io_base + ATA_ERR);
    log(Debug, "ATA error : 0x%x\n", err);
    if (err & ATA_ER_BBK)
    {
        return DiskError::BadSector;
    }
    if (err & ATA_ER_MC || err & ATA_ER_MCR)
    {
        return DiskError::NoMedia;
    }
    if (err & ATA_ER_ABRT)
    {
        return DiskError::Aborted;
    }
    else
    {
        warn("Unknown ATA error on port 0x%x : 0x%x\n", dev.port, err);
        return DiskError::Unknown;
    }
}

void cache_flush(const ata_device& dev)
{
    select(dev, 0, 0);

    outb(dev.port + ATA_CMD, ata_flush_ext);

    poll_bsy(dev);
}

bool detect(const ata_device& dev)
{
    outb(dev.control_base + ATA_DEVCTRL, 0); // clear control byte

    if (inb(dev.io_base + ATA_STATUS) == 0xFF)
    {
        log(Notice, "No ATA device on port 0x%x\n", dev.io_base);
        return false;
    }

    soft_reset(dev);
    wait_400ns(dev);

    outb(dev.io_base + ATA_SELECT, dev.type == Master ? 0xA0 : 0xB0);
    wait_400ns(dev);

    poll_bsy(dev);

    uint8_t lba_lo = inb(dev.io_base + ATA_LBAMID);
    uint8_t lba_hi = inb(dev.io_base + ATA_LBAHI);

#if 1
    // test if a disk is present by writing to the CHS registers
    outb(dev.io_base + ATA_LBALO, 0x80); // magic constants, no meaning
    outb(dev.io_base + ATA_LBAMID, 0x81);
    outb(dev.io_base + ATA_SECCOUNT, 0x8b);
    if (inb(dev.io_base + ATA_LBALO) != 0x80)
    {
        log(Notice, "No disk present on port 0x%x\n", dev.io_base);
        return false;
    }
#endif

    if ((lba_lo == 0 && lba_hi == 0) || (lba_lo == 0x3c && lba_hi == 0xc3))
    {
        return true;
    }
    else if (lba_lo == 0x14 && lba_hi == 0xeb)
    {
        log(Notice, "ATA is an ATAPI device, ignoring (lba lo 0x%x, hi 0x%x)\n", lba_lo, lba_hi);
        return false;
    }
    else
    {
        log(Notice, "ATA is not an ATA device, ignoring (lba lo 0x%x, hi 0x%x)\n", lba_lo, lba_hi);
        return false;
    }
}

void soft_reset(const ata_device& dev)
{
    outb(dev.control_base + ATA_DEVCTRL, 0x4); // send RST command
    wait_400ns(dev);
    outb(dev.control_base + ATA_DEVCTRL, 0x0); // clear RST bit
    wait_400ns(dev);
}

IDEDisk::IDEDisk(const ata_device& dev)
    : ::DiskImpl<IDEDisk>()
{
    m_dev = dev;
}

size_t IDEDisk::disk_size() const
{
    if (!m_id_data) update_id_data();

    if (m_id_data) return m_id_data->sectors_48*512;
    else return 0;
}

size_t IDEDisk::sector_size() const
{
    if (!m_id_data) update_id_data();

    if (m_id_data) return m_id_data->sector_size*2?:512;
    else return 512;
}

kpp::string IDEDisk::drive_name() const
{
    if (!m_id_data) update_id_data();

    if (m_id_data) return ata_string(m_id_data->model);
    else return "<invalid>";
}

void IDEDisk::update_id_data() const
{
    m_id_data = identify(m_dev);

    if (!m_id_data)
    {
        warn("IDE PIO disk 0x%x/0x%x returned invalid identify data\n", m_dev.io_base, m_dev.type);
    }
}

void IDEDisk::flush_hardware_cache()
{
    cache_flush(m_dev);
}

}
