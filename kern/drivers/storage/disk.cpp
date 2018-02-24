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

#include "power/powermanagement.hpp"

// TODO : Locks

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
    const size_t sect_size = sector_size();

    const size_t sector = offset / sect_size;
    const size_t count = size / sect_size + (size%sect_size?1:0);
    auto data = read_cache_sector(sector, count);
    assert(data.size() >= size);

    offset %= sect_size;

    return MemBuffer{data.begin() + offset, data.begin() + offset + size};
}

MemBuffer Disk::read() const
{
    return read(0, disk_size());
}

void Disk::write(size_t offset, gsl::span<const uint8_t> data)
{
    if (read_only()) return;

    const size_t sect_size = sector_size();

    const size_t base = offset / sect_size;
    const size_t byte_offset = offset % sect_size;
    const size_t count = data.size() / sect_size + (data.size()%sect_size?1:0);

    auto chunks = split(data, sect_size);

    for (size_t i { 0 }; i < count; ++i)
    {
        if (i == 0 || i == count-1)
        {
            auto sect_data = read_cache_sector(i + base, 1);
            std::copy(chunks[i].begin(), chunks[i].end(),
                    sect_data.begin() + byte_offset);

            write_cache_sector(i + base, sect_data);
        }
        else if (i == count-1)
        {
            auto sect_data = read_cache_sector(i + base, 1);
            std::copy(chunks[i].begin(), chunks[i].end(),
                    sect_data.begin());

            write_cache_sector(i + base, sect_data);
        }
        else
        {
            write_cache_sector(i + base, chunks[i]);
        }
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
    enable_caching(false);
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
