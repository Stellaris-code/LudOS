/*
mbr.hpp

Copyright (c) 30 Yann BOUCHER (yann)

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
#ifndef MBR_HPP
#define MBR_HPP

#include <stdint.h>

#include <vector.hpp>

#include "drivers/storage/disk.hpp"

class Disk;

namespace mbr
{

struct Partition : public DiskSlice
{
    Partition(Disk& disk, size_t offset, size_t size)
        : DiskSlice(disk, offset, size)
    {
        //enable_caching(false);
    }

    template <typename... Args>
    static Partition& create_disk(Args&&... args)
    {
        return DiskImpl<Partition>::create_disk(std::forward<Args>(args)...);
    }

    struct [[gnu::packed]] Data
    {
        uint8_t boot_flag;
        uint8_t start_head;
        uint16_t start_sector : 6;
        uint16_t start_cylinder : 10;
        uint8_t system_id;
        uint8_t end_head;
        uint16_t end_sector : 6;
        uint16_t end_cylinder : 10;
        uint32_t relative_sector;
        uint32_t sector_count;
    } data;

    // Additional data
    uint8_t partition_number;
};

ref_vector<Partition> read_partitions(Disk& drive);

}

#endif // MBR_HPP
