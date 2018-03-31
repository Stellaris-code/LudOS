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

#include "utils/stlutils.hpp"
#include "utils/messagebus.hpp"
#include "utils/memutils.hpp"

#include "power/powermanagement.hpp"
#include "time/timer.hpp"
#include "tasking/spinlock.hpp"

DECLARE_LOCK(read_lock);
DECLARE_LOCK(write_lock);

void Disk::system_init()
{
    MessageBus::register_handler<SyncDisksCache>([](const SyncDisksCache&)
    {
        for (Disk& disk : disks())
        {
            disk.flush_cache();
        }
    });

    MessageBus::register_handler<ShutdownMessage>([](const ShutdownMessage&)
    {
        MessageBus::send(SyncDisksCache{});
    });
}

Disk::Disk()
    : m_cache(*this)
{
    enable_caching(true);
}

Disk::~Disk()
{
}

bool Disk::read_only() const
{
    return m_read_only;
}

void Disk::set_read_only(bool val)
{
    m_read_only = val;
}

MemBuffer Disk::read(size_t offset, size_t size) const
{
    LOCK(read_lock);

    const size_t sect_size = sector_size();

    const size_t sector = offset / sect_size;
    const size_t count = ((offset+size)/sect_size + ((offset+size)%sect_size?1:0))
            - (offset/sect_size + (offset%sect_size?1:0))
            + ((offset+size)%sect_size?1:0);

    assert(count <= disk_size()/sector_size() + (disk_size()%sector_size()?1:0));

    auto data = read_cache_sector(sector, count);
    assert(data.size() >= size);

    offset %= sect_size;

    assert(data.size() >= offset + size);

    UNLOCK(read_lock);

    return MemBuffer{data.begin() + offset, data.begin() + offset + size};
}

MemBuffer Disk::read() const
{
    return read(0, disk_size());
}

void Disk::write_offseted_sector(size_t base, size_t byte_off, gsl::span<const uint8_t> data)
{
    LOCK(write_lock);

    assert((size_t)data.size() <= sector_size());
    assert(data.size() + byte_off <= disk_size());

    gsl::span<const uint8_t> spans[2];
    spans[0] = data.subspan(0, std::min<size_t>(sector_size() - byte_off, data.size()));

    if (sector_size() - byte_off < (size_t)data.size())
    {
        spans[1] = data.subspan(sector_size() - byte_off);
    }

    for (size_t i { 0 }; i < 2; ++i)
    {
        auto sect_data = read_cache_sector(base+i, 1);

        assert((size_t)spans[i].size() <= sect_data.size() - byte_off);

        std::copy(spans[i].begin(), spans[i].end(), sect_data.begin() + byte_off);
        write_cache_sector(base+i, sect_data);

        byte_off = 0;
    }

    UNLOCK(write_lock);
}

void Disk::write(size_t offset, gsl::span<const uint8_t> data)
{
    if (read_only()) return;

    const size_t sect_size = sector_size();

    const size_t base = offset / sect_size;
    const size_t count = data.size() / sect_size + (data.size()%sect_size?1:0);

    auto chunks = split(data, sect_size);

    for (size_t i { 0 }; i < count; ++i)
    {
        write_offseted_sector(base + i, offset % sect_size, chunks[i]);
        offset -= sect_size;
    }
}

void Disk::enable_caching(bool val)
{
    if (!val && m_caching)
    {
        flush_cache();
    }

    m_caching = val;
}

void Disk::flush_cache()
{
    m_cache.flush();
    flush_hardware_cache();
}

MemBuffer Disk::read_cache_sector(size_t sector, size_t count) const
{
    if (m_caching) return m_cache.read_sector(sector, count);
    else return read_sector(sector, count);
}

void Disk::write_cache_sector(size_t sector, gsl::span<const uint8_t> data)
{

    if (m_caching) m_cache.write_sector(sector, data);
    else write_sector(sector, data);
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

MemoryDisk::MemoryDisk(uint8_t* data, size_t size, const std::string& name)
    : DiskImpl<MemoryDisk>(), m_size(size), m_data(data), m_name(name)
{
    enable_caching(false);
}

MemoryDisk::MemoryDisk(const uint8_t *data, size_t size, const std::string& name)
    : MemoryDisk(const_cast<uint8_t*>(data), size, name)
{
    m_const = true;
}

MemBuffer MemoryDisk::read_sector(size_t sector, size_t count) const
{
    MemBuffer data(count * sector_size());

    const size_t offset = sector * sector_size();

    std::copy(m_data + offset, m_data + offset + count * sector_size(), data.begin());

    return data;
}

void MemoryDisk::write_sector(size_t sector, gsl::span<const uint8_t> data)
{
    if (m_const)
    {
        throw DiskException(*this, DiskException::ReadOnly);
    }

    const size_t offset = sector * sector_size();

    std::copy(data.begin(), data.end(), m_data + offset);
}

DiskSlice::DiskSlice(Disk &disk, size_t offset, size_t size)
    : DiskImpl<DiskSlice>(), m_base_disk(disk), m_offset(offset), m_size(size)
{
}

MemBuffer DiskSlice::read_sector(size_t sector, size_t count) const
{
    if (sector + count > m_size)
    {
        throw DiskException(*this, DiskException::OutOfBounds);
    }

    return m_base_disk.read_sector(sector + m_offset, count);
}

void DiskSlice::write_sector(size_t sector, gsl::span<const uint8_t> data)
{
    if (sector + data.size()/sector_size() > m_size)
    {
        throw DiskException(*this, DiskException::OutOfBounds);
    }

    m_base_disk.write_sector(sector + m_offset, data);
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

    disk.enable_caching(false);

    for (size_t i { 0 }; i < 100; ++i)
    {
        disk.write(i*160, data);

        disk.flush_cache();

        auto result = disk.read(i*160, data.size());
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

    disk.enable_caching(caching_state);
}
