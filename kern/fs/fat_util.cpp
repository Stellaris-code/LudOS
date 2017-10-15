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

    size_t next_cluster = FAT_entry(FAT, info, entry_clusters.back());

    for (auto cluster : clusters)
    {

    }
}
}
