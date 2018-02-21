/*
mbr.cpp

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

#include "mbr.hpp"

#include <array.hpp>

#include "utils/memutils.hpp"

ref_vector<mbr::Partition> mbr::read_partitions(Disk& disk)
{
    if(!disk.caching_enabled()){err("%s : %d\n", disk.drive_name().c_str(), disk.is_partition());}

    ref_vector<Partition> partitions;

    auto buf = disk.read(0, 512);

    if (buf.empty()) return {};
    if (buf[0x1fe] != 0x55 && buf[0x1ff] != 0xAA)
    {
        return {}; // invalid signature
    }

    std::vector<uint16_t> addresses = {static_cast<uint16_t>(0x1BE), static_cast<uint16_t>(0x1CE),
                                       static_cast<uint16_t>(0x1DE), static_cast<uint16_t>(0x1EE)};
    for (size_t i { 0 }; i < addresses.size(); ++i)
    {
        Partition::Data data = *reinterpret_cast<Partition::Data*>(buf.data() + addresses[i]);

        if (data.boot_flag == 0x80)
        {
            auto& ref = Partition::create_disk(disk, (size_t)data.relative_sector, (size_t)data.sector_count);
            ref.partition_number = i+1;
            ref.data = data;

            partitions.emplace_back(ref);
        }
    }

    return partitions;
}
