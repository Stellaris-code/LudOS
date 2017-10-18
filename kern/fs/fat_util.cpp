/*
fat_util.cpp

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

#include "fat.hpp"

#include "panic.hpp"

#include "utils/bitops.hpp"

namespace fat::detail
{
std::vector<uint32_t> find_free_clusters(const FATInfo &info, size_t clusters)
{
    std::vector<uint32_t> clusters_vec;

    auto FAT = get_FAT(info);

    for (size_t cluster { 0 }; cluster < info.total_clusters; ++cluster)
    {
        uint32_t table_value = FAT_entry(FAT, info, cluster);

        if (table_value == 0)
        {
            //log("at 0x%x : 0x%x\n", cluster, table_value);
            clusters_vec.emplace_back(cluster);
        }

        if (clusters_vec.size() == clusters)
        {
            return clusters_vec;
        }
    }

    return clusters_vec;
}

void add_clusters(const FATInfo& info, const Entry& entry, const std::vector<uint32_t>& clusters)
{
    auto entry_clusters = get_cluster_chain(entry.low_cluster_bits | (entry.high_cluster_bits << 16), info);

    auto FAT = get_FAT(info);

    size_t prev_cluster = entry_clusters.back();

    for (auto cluster : clusters)
    {
        set_FAT_entry(FAT, info, prev_cluster, cluster);
        //log("FAT at 0x%x is 0x%x/0x%x\n", prev_cluster, FAT_entry(FAT, info, prev_cluster), cluster);
        prev_cluster = cluster;
    }

    size_t end_of_chain;
    switch (info.type)
    {
        case FATType::FAT12:
            end_of_chain = 0xFFF;
            break;
        case FATType::FAT16:
            end_of_chain = 0xFFFF;
            break;
        case FATType::FAT32:
        default:
            end_of_chain = 0x0FFFFFFF;
            break;
    }

    set_FAT_entry(FAT, info, prev_cluster, end_of_chain);
    //log("FAT at 0x%x is 0x%x/0x%x\n", prev_cluster, FAT_entry(FAT, info, prev_cluster), end_of_chain);

    write_FAT(FAT, info);
}

size_t clusters(const FATInfo& info, size_t byte_size)
{
    const size_t bytes_per_cluster = info.bootsector.bytes_per_sector*info.bootsector.sectors_per_cluster;
    return byte_size/bytes_per_cluster + (byte_size%bytes_per_cluster != 0);
}

void free_cluster_chain(const FATInfo& info, size_t first_cluster)
{
    auto cluster_chain = get_cluster_chain(first_cluster, info);
    auto FAT = get_FAT(info);

    size_t end_of_chain;
    switch (info.type)
    {
        case FATType::FAT12:
            end_of_chain = 0xFFF;
            break;
        case FATType::FAT16:
            end_of_chain = 0xFFFF;
            break;
        case FATType::FAT32:
        default:
            end_of_chain = 0x0FFFFFFF;
            break;
    }

    for (auto clu : cluster_chain)
    {
        if (clu == cluster_chain.front())
        {
            set_FAT_entry(FAT, info, clu, end_of_chain);
        }
        else
        {
            set_FAT_entry(FAT, info, clu, 0); // mark as free
        }
    }

    write_FAT(FAT, info);
}

void set_dirty_bit(FATInfo info, bool value)
{
    if (info.type == FATType::FAT12 || info.type == FATType::FAT16)
    {
        info.ext16.dirty_flag = value;
    }
    else if (info.type == FATType::FAT32)
    {
        info.ext32.dirty_flag = value;
    }

    auto FAT = get_FAT(info);
    auto second_entry = FAT_entry(FAT, info, 1);

    if (info.type == FATType::FAT16)
    {
        bit_change(second_entry, 15, value);
    }
    else if (info.type == FATType::FAT32)
    {
        bit_change(second_entry, 27, value);
    }
    set_FAT_entry(FAT, info, 1, second_entry);

    write_FAT(FAT, info);

    write_bs(info);
}
}
