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

void select(uint16_t port, uint8_t type, uint64_t block, uint16_t count)
{
    outb(port + 6, 0x40 | type);

    outb(port + 2, (count >> 8) & 0xFF);

    outb(port + 3, (block >> 24) & 0xFF);
    outb(port + 4, (block >> 32) & 0xFF);
    outb(port + 5, (block >> 40) & 0xFF);

    outb(port + 2, count & 0xFF);

    outb(port + 3, block & 0xFF);
    outb(port + 4, (block >> 8) & 0xFF);
    outb(port + 5, (block >> 16) & 0xFF);

    // 400ns
    for (size_t i { 0 }; i < 5; ++i)
    {
        inb(port + 6);
    }
}

kpp::optional<identify_data> identify(uint16_t port, uint8_t type)
{
    select(port, type == Master ? 0xA0 : 0xB0, 0, 0);

    outb(port + 7, 0xEC);

    const char* port_name;
    if (port == Primary)
    {
        port_name = "Primary";
    }
    else if (port == Secondary)
    {
        port_name = "Secondary";
    }
    else if (port == Third)
    {
        port_name = "Third";
    }
    else
    {
        port_name = "Fourth";
    }

    uint8_t status = inb(port + 7);
    if (status)
    {

        poll(port);
        do
        {
            status = inb(port + 7);
            if(status & 0x01)
            {
                log(Debug, "ATA %s%s has ERR set. Disabled.\n", port_name, type==Master?" master":" slave");
                return {};
            }
        } while(!(status & 0x08));

        log(Debug, "ATA %s%s is online.\n", port_name, type==Master?" master":" slave");

        kpp::array<uint16_t, 256> buffer;

        poll(port);

        for(size_t i = 0; i<256; i++)
        {
            buffer[i] = inw(port + 0);
        }

        identify_data* id_data;
        id_data = reinterpret_cast<identify_data*>(buffer.data());

        log(Debug, "Firmware : %s, model : %s\n", ata_string(id_data->firmware).c_str(), ata_string(id_data->model).c_str());

        return *id_data;
    }
    else
    {
        log(Debug, "ATA %s%s doesn't exist.\n", port_name, type==Master?" master":" slave");

        return {};
    }
}

void poll(uint16_t port)
{
    size_t max_iters { 0x10000 };
    while (!(inb(port + 7) & 0x08) && max_iters-- > 0) { nop(); }
}

void poll_bsy(uint16_t port)
{
    size_t max_iters { 0x10000 };
    while ((inb(port + 7) & 0x80) && max_iters-- > 0) { nop(); }
}

bool flush(uint16_t port)
{
    poll_bsy(port);
    poll(port);

    outb(port + 7, 0xEA);

    poll_bsy(port);
    poll(port);

    if (error_set(port))
    {
        bool stat = !error_set(port);
        if (!stat)
        {
            clear_error(port);

            return stat;
        }
    }

    return true;
}

bool error_set(uint16_t port)
{
    return bit_check(status_register(port), 0) || bit_check(status_register(port), 5);
}

void clear_error(uint16_t port)
{
    outb(port + 7, 0x00); // nop
}

uint8_t error_register(uint16_t port)
{
    return inb(port + 1);
}

uint8_t status_register(uint16_t port)
{
    return inb(port + 7);
}

uint8_t drive_register(uint16_t port)
{
    return inb(port + 6);
}

DiskError::Type get_error(uint16_t port)
{
    auto err = error_register(port);
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
        return DiskError::Unknown;
    }
}

void cache_flush(uint16_t port, uint8_t type)
{
    select(port, type == Master ? 0xA0 : 0xB0, 0, 0);

    outb(port + 7, 0xEA);

    poll_bsy(port);
}

IDEDisk::IDEDisk(uint16_t port, uint8_t type)
    : ::DiskImpl<IDEDisk>()
{
    m_port = port;
    m_type = type;
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
    m_id_data = identify(m_port, m_type);

    if (!m_id_data)
    {
        warn("IDE PIO disk 0x%x/0x%x returned invalid identify data\n", m_port, m_type);
    }
}

void IDEDisk::flush_hardware_cache()
{
    cache_flush(m_port, m_type);
}

}
