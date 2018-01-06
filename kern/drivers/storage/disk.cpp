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

// TODO : Locks

std::vector<uint8_t> Disk::read(size_t offset, size_t size) const
{
    const size_t sect_size = sector_size();

    const size_t sector = offset / sect_size;
    const size_t count = size / sect_size + (size%sect_size?1:0);

    auto data = read_sector(sector, count);
    assert(data.size() >= size);
    data.resize(size);

    return data;
}

std::vector<uint8_t> Disk::read() const
{
    return read(0, disk_size());
}

void Disk::write(size_t offset, std::vector<uint8_t> data)
{
    const size_t sect_size = sector_size();

    const size_t sector = offset / sect_size;
    const size_t count = data.size() / sect_size + (data.size()%sect_size?1:0);

    // fill the last chunk with disk data if it's too small
    if (data.size() % sect_size)
    {
        auto sect = read_sector(sector + count, 1);
        const size_t sect_off = data.size() / sect_size;
        std::copy(data.begin() + sect_off*sect_size, data.end(), sect.begin());
        data.resize(count * sect_size);
        std::copy(sect.begin(), sect.end(), data.begin() + sect_off*sect_size);
    }

    write_sector(sector, data);
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
    : m_size(size), m_data(data), m_name(name)
{

}

MemoryDisk::MemoryDisk(const uint8_t *data, size_t size, const std::string& name)
    : MemoryDisk(const_cast<uint8_t*>(data), size, name)
{
    m_const = true;
}

std::vector<uint8_t> MemoryDisk::read_sector(size_t sector, size_t count) const
{
    std::vector<uint8_t> data(count * sector_size());

    const size_t offset = sector * sector_size();

    std::copy(m_data + offset, m_data + offset + count * sector_size(), data.begin());

    return data;
}

void MemoryDisk::write_sector(size_t sector, const std::vector<uint8_t> &data)
{
    if (m_const)
    {
        throw DiskException(DiskException::ReadOnly);
    }

    const size_t offset = sector * sector_size();

    std::copy(data.begin(), data.end(), m_data + offset);
}
