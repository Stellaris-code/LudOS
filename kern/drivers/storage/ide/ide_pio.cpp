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

bool ide::pio::detail::read_one(const ata_device &dev, uint64_t block, uint16_t *buf)
{
    select(dev, block, 1);
    outb(dev.io_base + ATA_CMD, 0x24);

    poll_bsy(dev);
//   poll(port);

#if 0
    insw(port + 0, buf, 256);
#else
    for (size_t i { 0 }; i < 256; ++i)
    {
        *buf++ = inw(dev.io_base + ATA_DATA);
    }
#endif

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


bool ide::pio::detail::write_one(const ata_device& dev, uint64_t block, const uint16_t *buf)
{
    select(dev, block, 1);

    outb(dev.io_base + ATA_CMD, 0x34);

    poll_bsy(dev);

#if 0
    outsw(port + 0, buf, 256);
#else
    for (size_t i { 0 }; i < 256; ++i)
    {
        outw(dev.io_base + ATA_CMD, *buf++);
    }
#endif

    if (error_set(dev))
    {
        bool stat = !error_set(dev);
        if (!stat)
        {
            clear_error(dev);

            return stat;
        }
    }

    return flush(dev);
}

uint16_t control_port(ide::BusPort port)
{
    switch (port)
    {
        case ide::Primary:
            return 0x3F6;
        case ide::Secondary:
            return 0x376;
        case ide::Third:
            return 0x3E6;
        case ide::Fourth:
            return 0x366;
        default:
            assert(false);
    }
}


void ide::pio::init()
{
    for (auto bus : {Primary, Secondary, Third, Fourth})
    {
        for (auto type : {Master, Slave})
        {
            ata_device dev = {bus, type, (uint16_t)bus, control_port(bus)};
            if (identify(dev))
            {
                ide::pio::Disk::create_disk(dev);
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
            if (identify({bus, type, (uint16_t)bus, control_port(bus)}))
            {
                result.emplace_back(bus, type);
            }
        }
    }
    return result;
}


bool ide::pio::read(const ata_device& dev, uint64_t block, size_t count, uint16_t *buf)
{
    for (size_t i { 0 }; i < count; ++i)
    {
        if (!detail::read_one(dev, block + i, buf)) return false;
        buf += 256;
    }

    return true;
}

bool ide::pio::write(const ata_device& dev, uint64_t block, size_t count, const uint16_t *buf)
{
    for (size_t i { 0 }; i < count; ++i)
    {
        if (!detail::write_one(dev, block + i, buf)) return false;
        buf += 256;
    }
    return true;
}

[[nodiscard]]
kpp::expected<MemBuffer, DiskError> ide::pio::Disk::read_sector(size_t sector, size_t count) const
{
    MemBuffer data(count * sector_size());
    if (ide::pio::read(m_dev, sector, count, (uint16_t*)data.data()))
    {
        return std::move(data);
    }
    else
    {
        return kpp::make_unexpected(DiskError{get_error(m_dev)});
    }
}

[[nodiscard]]
kpp::expected<kpp::dummy_t, DiskError> ide::pio::Disk::write_sector(size_t sector, gsl::span<const uint8_t> data)
{
    const size_t count = data.size() / sector_size() + (data.size()%sector_size()?1:0);

    if (!ide::pio::write(m_dev, sector, count, (const uint16_t*)data.data()))
    {
        return kpp::make_unexpected(DiskError{get_error(m_dev)});
    }

    return {};
}

ide::pio::Disk::Disk(const ata_device &dev)
    : IDEDisk(dev)
{
}
