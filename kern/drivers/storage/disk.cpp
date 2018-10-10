/*
disk.cpp

Copyright (c) 03 Yann BOUCHER (yann)

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

#include "disk.hpp"

#include <vector.hpp>
#include "utils/kmsgbus.hpp"

#include "utils/stlutils.hpp"
#include "utils/memutils.hpp"
#include "utils/env.hpp"

#include "power/powermanagement.hpp"
#include "time/timer.hpp"

void Disk::system_init()
{
    kmsgbus.register_handler<SyncDisksCache>([](const SyncDisksCache&)
    {
        for (Disk& disk : disks())
        {
            auto result = disk.flush_cache();
            if (!result)
                err("Could not flush disk %s : %s\n", disk.drive_name().c_str(), result.error().to_string());
        }
    });

    kmsgbus.register_handler<ShutdownMessage>([](const ShutdownMessage&)
    {
        kmsgbus.send(SyncDisksCache{});
    });
}

Disk::Disk()
    : m_cache(*this)
{
    if (!kgetenv("no_caching")) (void)enable_caching(true);
}

Disk::~Disk() = default;

bool Disk::read_only() const
{
    return m_read_only;
}

void Disk::set_read_only(bool val)
{
    m_read_only = val;
}

kpp::expected<MemBuffer, DiskError> Disk::read(size_t offset, size_t size) const
{
    MemBuffer buf;
    buf.resize(size);

    auto result = read(offset, buf);
    if (!result) return kpp::make_unexpected(result.error());

    return std::move(buf);
}

kpp::expected<kpp::dummy_t, DiskError> Disk::read(size_t offset, gsl::span<uint8_t> data) const
{
    const size_t sect_size = sector_size();

    const size_t first_stride_size = offset%sect_size;
    const size_t second_stride_size = (offset+data.size()) % sect_size;
    const size_t mid_stride_size = data.size() - ((sect_size - first_stride_size)%sect_size) - second_stride_size;

    if (first_stride_size != 0) // offset isn't aligned, manually fetch the first sector
    {
        MemBuffer buf(sect_size);
        auto result = read_cache_sectors(offset/sect_size, buf);
        if (!result) return kpp::make_unexpected(result.error());
        std::copy(buf.begin() + first_stride_size, buf.begin() + sect_size, data.begin());
    }
    if (second_stride_size != 0) // size isn't aligned, manually fetch the last sector
    {
        MemBuffer buf(sect_size);
        auto result = read_cache_sectors((offset+data.size())/sect_size, buf);
        if (!result) return kpp::make_unexpected(result.error());
        std::copy(buf.begin(), buf.begin() + second_stride_size, data.begin() + (data.size()-second_stride_size));
    }

    if (mid_stride_size != 0)
    {
        gsl::span<uint8_t> aligned_span = {data.data() + (sect_size - first_stride_size)%sect_size,
                                           (long)mid_stride_size};

        assert(aligned_span.size() % sect_size == 0);

        auto result = read_cache_sectors(offset/sect_size, aligned_span);
        if (!result) return kpp::make_unexpected(result.error());
    }

    if (first_stride_size || second_stride_size)
    {
        log_serial("Unaligned disk read\n");
    }

    return {};
}

kpp::expected<MemBuffer, DiskError> Disk::read() const
{
    return read(0, disk_size());
}

[[nodiscard]]
kpp::expected<kpp::dummy_t, DiskError> Disk::write_offseted_sector(size_t base, size_t byte_off, gsl::span<const uint8_t> data)
{
    const size_t sector_size = this->sector_size();

    assert((size_t)data.size() <= sector_size);
    assert(data.size() + byte_off <= disk_size());

    gsl::span<const uint8_t> spans[2];
    spans[0] = data.subspan(0, std::min<size_t>(sector_size - byte_off, data.size()));

    if (sector_size - byte_off < (size_t)data.size())
    {
        spans[1] = data.subspan(sector_size - byte_off);
    }

    uint8_t sect_data[sector_size];
    gsl::span sect_span = {sect_data, (long)sector_size};
    for (size_t i { 0 }; i < 2; ++i)
    {
        auto result = read_cache_sectors(base+i, sect_span);
        assert(result);

        assert((size_t)spans[i].size() <= sect_span.size() - byte_off);

        std::copy(spans[i].begin(), spans[i].end(), sect_data + byte_off);
        auto write_result = write_cache_sectors(base+i, sect_span);
        if (!write_result) return write_result;

        byte_off = 0;
    }

    return {};
}

kpp::expected<kpp::dummy_t, DiskError> Disk::write(size_t offset, gsl::span<const uint8_t> data)
{
    if (read_only()) return kpp::make_unexpected(DiskError{DiskError::ReadOnly});

    const size_t sect_size = sector_size();

    const size_t base = offset / sect_size;
    const size_t count = data.size() / sect_size + (data.size()%sect_size?1:0);

    auto chunks = split(data, sect_size);

    for (size_t i { 0 }; i < count; ++i)
    {
        auto result = write_offseted_sector(base + i, offset % sect_size, chunks[i]);
        if (!result) return result;
        offset -= sect_size;
    }

    return {};
}

[[nodiscard]]
kpp::expected<kpp::dummy_t, DiskError> Disk::enable_caching(bool val)
{
    if (!val && m_caching)
    {
        auto result = flush_cache();
        warn("Error flushing cache on disk '%s' : %s\n", drive_name().c_str(), result.error().to_string());
        return {};
    }

    // TODO : investigate why enabling caching destroys performance !
    //m_caching = val;

    return {};
}

[[nodiscard]]
kpp::expected<kpp::dummy_t, DiskError> Disk::flush_cache()
{
    auto result = m_cache.flush();
    if (!result) return result;
    flush_hardware_cache();

    return {};
}

kpp::expected<kpp::dummy_t, DiskError> Disk::read_cache_sectors(size_t sector, gsl::span<uint8_t> data) const
{
    if (m_caching) return m_cache.read_sectors(sector, data);
    else return read_sectors(sector, data);
}

[[nodiscard]]
kpp::expected<kpp::dummy_t, DiskError> Disk::write_cache_sectors(size_t sector, gsl::span<const uint8_t> data)
{
    if (read_only()) return kpp::make_unexpected(DiskError{DiskError::ReadOnly});

    if (m_caching) return m_cache.write_sectors(sector, data);
    else return write_sectors(sector, data);
}

ref_vector<Disk> Disk::disks()
{
    ref_vector<Disk> vec;
    for (auto& disk : m_disks)
    {
        vec.emplace_back(*disk);
    }

    return vec;
}

MemoryDisk::MemoryDisk(uint8_t* data, size_t size, kpp::string name)
    : DiskImpl<MemoryDisk>(), m_size(size), m_data(data), m_name(std::move(name))
{
    (void)enable_caching(false);
}

MemoryDisk::MemoryDisk(const uint8_t *data, size_t size, const kpp::string& name)
    : MemoryDisk(const_cast<uint8_t*>(data), size, name)
{
    m_const = true;
}

kpp::expected<kpp::dummy_t, DiskError> MemoryDisk::read_sectors(size_t sector, gsl::span<uint8_t> data) const
{
    const size_t offset = sector * sector_size();
    const size_t count  = data.size() / sector_size();

    std::copy(m_data + offset, m_data + offset + count * sector_size(), data.begin());

    return {};
}

[[nodiscard]]
kpp::expected<kpp::dummy_t, DiskError> MemoryDisk::write_sectors(size_t sector, gsl::span<const uint8_t> data)
{
    if (m_const)
    {
        return kpp::make_unexpected(DiskError{DiskError::ReadOnly});
    }

    const size_t offset = sector * sector_size();

    std::copy(data.begin(), data.end(), m_data + offset);

    return {};
}

DiskSlice::DiskSlice(Disk &disk, size_t offset, size_t size)
    : DiskImpl<DiskSlice>(), m_base_disk(disk), m_offset(offset), m_size(size)
{
    set_read_only(disk.read_only());
}

kpp::expected<kpp::dummy_t, DiskError> DiskSlice::read_sectors(size_t sector, gsl::span<uint8_t> data) const
{
    const size_t count = data.size()/sector_size();
    if (sector + count > m_size)
    {
        return kpp::make_unexpected(DiskError{DiskError::OutOfBounds});
    }

    return m_base_disk.read_sectors(sector + m_offset, data);
}

[[nodiscard]]
kpp::expected<kpp::dummy_t, DiskError> DiskSlice::write_sectors(size_t sector, gsl::span<const uint8_t> data)
{
    if (sector + data.size()/sector_size() > m_size)
    {
        return kpp::make_unexpected(DiskError{DiskError::OutOfBounds});
    }

    return m_base_disk.write_sectors(sector + m_offset, data);
}

void test_writes(Disk &disk)
{
    log(Notice, "Testing writes on disk %s\n", disk.drive_name().c_str());

    std::vector<uint8_t> data(disk.sector_size()*11+23);
    for (size_t i { 0 }; i < data.size(); ++i)
    {
        data[i] = i % 0x100;
    }

    bool caching_state = disk.caching_enabled();

    (void)disk.enable_caching(false);

    for (size_t i { 0 }; i < 100; ++i)
    {
        if (!disk.write(i*160, data))
        {
            err("Disk write test error\n");
            return;
        }

        if (!disk.flush_cache())
        {
            err("Disk write test error\n");
            return;
        }

        auto expected = disk.read(i*160, data.size());
        if (!expected)
        {
            err("Error on disk write test : %s\n", expected.error().to_string());
            return;
        }

        auto result = std::move(expected.value());

        assert(result.size() == data.size());

        for (size_t j { 0 }; j < result.size(); ++j)
        {
            if (result[j] != data[j])
            {
                warn("Difference at %d (0x%x, 0x%x)\n", j, data[j], result[j]);
                break;
            }
        }
    }

    (void)disk.enable_caching(caching_state);
}
