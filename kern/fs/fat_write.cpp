/*
fat_write.cpp

Copyright (c) 14 Yann BOUCHER (yann)

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

#include "fat.hpp"

#include "panic.hpp"

#include "utils/stlutils.hpp"
#include "utils/logging.hpp"
#include "drivers/diskinterface.hpp"

namespace fat::detail
{

void fat_file::update_entry() const
{

}

bool write_cluster(const FATInfo& info, size_t cluster, const std::vector<uint8_t>& data)
{
    assert(data.size() == info.bootsector.sectors_per_cluster * info.bootsector.bytes_per_sector);

    return DiskInterface::write(info.drive, first_sector_of_cluster(cluster, info) + info.base_sector, info.bootsector.sectors_per_cluster, data.data());
}

bool write(const fat::Entry &entry, const FATInfo &info, const std::vector<uint8_t>& data)
{
    if (data.size() > entry.size)
    {
        panic("Implement resizing");

        return false;
    }
    else
    {
        auto clusters = get_cluster_chain(entry.low_cluster_bits | (entry.high_cluster_bits << 16), info);
        auto data_chunks = split(data, info.bootsector.sectors_per_cluster * info.bootsector.bytes_per_sector * clusters.size(), true);

        for (size_t i { 0 }; i < std::min(data_chunks.size(), clusters.size()); ++i)
        {
            if (!write_cluster(info, clusters[i], data_chunks[i]))
            {
                return false;
            }
        }
        return true;
    }
}

};
