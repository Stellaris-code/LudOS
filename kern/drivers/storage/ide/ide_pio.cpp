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

bool ide::pio::detail::read_one(uint16_t port, uint8_t type, uint64_t block, uint16_t *buf)
{
    select(port, type, block, 1);
    outb(port + 7, 0x24);

    poll_bsy(port);
   //poll(port);

    insw(port + 0, buf, 256);

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


bool ide::pio::detail::write_one(uint16_t port, uint8_t type, uint64_t block, const uint16_t *buf)
{
    select(port, type, block, 1);

    outb(port + 7, 0x34);

    poll_bsy(port);

#if 0
    outsw(port + 0, buf, 256);
#else
    for (size_t i { 0 }; i < 256; ++i)
    {
        outw(port + 0, *buf++);
    }
#endif

    if (error_set(port))
    {
        bool stat = !error_set(port);
        if (!stat)
        {
            clear_error(port);

            return stat;
        }
    }

    return flush(port);
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


bool ide::pio::read(uint16_t port, uint8_t type, uint64_t block, size_t count, uint16_t *buf)
{
    for (size_t i { 0 }; i < count; ++i)
    {
        if (!detail::read_one(port, type, block + i, buf)) return false;
        buf += 256;
    }

    return true;
}

bool ide::pio::write(uint16_t port, uint8_t type, uint64_t block, size_t count, const uint16_t *buf)
{
    for (size_t i { 0 }; i < count; ++i)
    {
        if (!detail::write_one(port, type, block + i, buf)) return false;
        buf += 256;
    }
    return true;
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
    : IDEDisk(port, type)
{
}
