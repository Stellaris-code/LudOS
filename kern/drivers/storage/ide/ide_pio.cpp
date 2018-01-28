/*
ide_pio.cpp

Copyright (c) 15 Yann BOUCHER (yann)

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

#include "ide_pio.hpp"

#include "utils/logging.hpp"

#include "io.hpp"
#include "utils/bitops.hpp"
#include "utils/nop.hpp"
#include "utils/memutils.hpp"
#include "time/timer.hpp"

#include <array.hpp>
#include <string.hpp>

void ide::pio::detail::common(BusPort port, uint8_t type, uint64_t block, uint16_t count)
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

bool ide::pio::detail::read_one(BusPort port, ide::DriveType type, uint64_t block, uint16_t *buf)
{
    detail::common(port, type, block, 1);
    outb(port + 7, 0x24);

    detail::poll_bsy(port);
    detail::poll(port);

    insw(port + 0, buf, 256);

    if (detail::error_set(port))
    {
        bool stat = !detail::error_set(port);
        if (!stat)
        {
            detail::clear_error(port);

            return stat;
        }
    }

    return true;
}


bool ide::pio::detail::write_one(BusPort port, ide::DriveType type, uint64_t block, const uint16_t *buf)
{
    detail::common(port, type, block, 1);

    outb(port + 7, 0x34);

    detail::poll_bsy(port);

#if 0
    outsw(port + 0, buf, 256);
#else
    for (size_t i { 0 }; i < 256; ++i)
    {
        outw(port + 0, *buf++);
    }
#endif

    if (detail::error_set(port))
    {
        bool stat = !detail::error_set(port);
        if (!stat)
        {
            detail::clear_error(port);

            return stat;
        }
    }

    return flush(port);
}


void ide::pio::detail::poll(BusPort port)
{
    size_t max_iters { 0x10000 };
    while (!(inb(port + 7) & 0x08) && max_iters-- > 0) { nop(); }
}

void ide::pio::detail::poll_bsy(BusPort port)
{
    size_t max_iters { 0x10000 };
    while ((inb(port + 7) & 0x80) && max_iters-- > 0) { nop(); }
}

bool ide::pio::detail::flush(BusPort port)
{
    detail::poll_bsy(port);
    detail::poll(port);

    outb(port + 7, 0xEA);

    detail::poll_bsy(port);
    detail::poll(port);

    if (detail::error_set(port))
    {
        bool stat = !detail::error_set(port);
        if (!stat)
        {
            detail::clear_error(port);

            return stat;
        }
    }

    return true;
}

bool ide::pio::detail::error_set(BusPort port)
{
    return bit_check(status_register(port), 0) || bit_check(status_register(port), 5);
}

void ide::pio::detail::clear_error(BusPort port)
{
    outb(port + 7, 0x00); // nop
}

uint8_t ide::pio::error_register(BusPort port)
{
    return inb(port + 1);
}

uint8_t ide::pio::status_register(BusPort port)
{
    return inb(port + 7);
}

void ide::pio::init()
{
    for (auto bus : {Primary, Secondary, Third, Fourth})
    {
        for (auto type : {Master, Slave})
        {
            if (identify(bus, type))
            {
                ide::pio::Disk::create_disk(bus, type);
            }
        }
    }

    log(Info, "IDE PIO initialized.\n");
}

std::optional<ide::identify_data> ide::pio::identify(BusPort port, DriveType type)
{
    detail::common(port, type == Master ? 0xA0 : 0xB0, 0, 0);

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

        detail::poll(port);
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

        std::array<uint16_t, 256> buffer;

        detail::poll(port);

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

std::vector<std::pair<ide::BusPort, ide::DriveType>> ide::pio::scan()
{
    std::vector<std::pair<ide::BusPort, ide::DriveType>> result;

    for (auto bus : {Primary, Secondary, Third, Fourth})
    {
        for (auto type : {Master, Slave})
        {
            if (identify(bus, type))
            {
                result.emplace_back(bus, type);
            }
        }
    }
    return result;
}


bool ide::pio::read(BusPort port, DriveType type, uint64_t block, size_t count, uint16_t *buf)
{
    for (size_t i { 0 }; i < count; ++i)
    {
        if (!detail::read_one(port, type, block + i, buf)) return false;
        buf += 256;
    }
    return true;
}

bool ide::pio::write(BusPort port, DriveType type, uint64_t block, size_t count, const uint16_t *buf)
{
    for (size_t i { 0 }; i < count; ++i)
    {
        if (!detail::write_one(port, type, block + i, buf)) return false;
        buf += 256;
    }
    return true;
}

size_t ide::pio::Disk::disk_size() const
{
    if (!m_id_data) update_id_data();

    if (m_id_data) return m_id_data->sectors_48*512;
    else return 0;
}

size_t ide::pio::Disk::sector_size() const
{
    if (!m_id_data) update_id_data();

    if (m_id_data) return m_id_data->sector_size*2?:512;
    else return 512;
}

std::string ide::pio::Disk::drive_name() const
{
    if (!m_id_data) update_id_data();

    if (m_id_data) return ata_string(m_id_data->model);
    else return "<invalid>";
}

void ide::pio::Disk::update_id_data() const
{
    m_id_data = identify(m_port, m_type);

    if (!m_id_data)
    {
        warn("IDE PIO disk %d/%d returned invalid identify data\n", m_port, m_type);
    }
}

std::vector<uint8_t> ide::pio::Disk::read_sector(size_t sector, size_t count) const
{
    std::vector<uint8_t> data(count * sector_size());
    if (ide::pio::read(m_port, m_type, sector, count, (uint16_t*)data.data()))
    {
        return data;
    }
    else
    {
        throw DiskException(*this, get_error(m_port));
    }
}

void ide::pio::Disk::write_sector(size_t sector, const std::vector<uint8_t> &data)
{
    const size_t count = data.size() / sector_size() + (data.size()%sector_size()?1:0);

    if (!ide::pio::write(m_port, m_type, sector, count, (const uint16_t*)data.data()))
    {
        throw DiskException(*this, get_error(m_port));
    }
}

ide::pio::Disk::Disk(BusPort port, DriveType type)
{
    m_port = port;
    m_type = type;
}

DiskException::ErrorType ide::pio::get_error(ide::BusPort port)
{
    auto err = error_register(port);
    if (err & ATA_ER_BBK)
    {
        return DiskException::BadSector;
    }
    if (err & ATA_ER_MC || err & ATA_ER_MCR)
    {
        return DiskException::NoMedia;
    }
    if (err & ATA_ER_ABRT)
    {
        return DiskException::Aborted;
    }
    else
    {
        return DiskException::Unknown;
    }
}
