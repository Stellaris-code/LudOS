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
#include "time/time.hpp"

namespace fat
{

void fat_file::write_entry() const
{
    if (!info.read_only)
    {
        std::vector<uint8_t> data(info.bootsector.bytes_per_sector * info.bootsector.sectors_per_cluster);

        if (!DiskInterface::read(info.drive, entry_first_sector + info.base_sector,
                                 info.bootsector.sectors_per_cluster, data.data()))
        {
            return;
        }

        memcpy(data.data() + entry_idx * sizeof(Entry), reinterpret_cast<uint8_t*>(&entry), sizeof(Entry));

        if (!DiskInterface::write(info.drive, entry_first_sector + info.base_sector,
                                  info.bootsector.sectors_per_cluster, data.data()))
        {
            return;
        }
    }
}

void fat_file::update_access_date() const
{
    auto current_time = Time::get_time_of_day();

    entry.last_access_day = current_time.day;
    entry.last_access_month = current_time.month;
    entry.last_access_year = current_time.year - 1980;

    write_entry();
}

void fat_file::update_modif_date() const
{
    auto current_time = Time::get_time_of_day();

    entry.last_modif_sec = current_time.sec / 2;
    entry.last_modif_min = current_time.min;
    entry.last_modif_hour = current_time.hour;
    entry.last_modif_day = current_time.day;
    entry.last_modif_month = current_time.month;
    entry.last_modif_year = current_time.year - 1980;

    write_entry();
}

void fat_file::set_creation_date() const
{
    auto current_time = Time::get_time_of_day();

    entry.sec = current_time.sec / 2;
    entry.min = current_time.min;
    entry.hour = current_time.hour;
    entry.day = current_time.day;
    entry.month = current_time.month;
    entry.year = current_time.year - 1980;

    write_entry();
}

namespace detail
{

bool write_cluster(const FATInfo& info, size_t cluster, const std::vector<uint8_t>& data)
{
    assert(data.size() == info.bootsector.sectors_per_cluster * info.bootsector.bytes_per_sector);

    return DiskInterface::write(info.drive, first_sector_of_cluster(cluster, info) + info.base_sector, info.bootsector.sectors_per_cluster, data.data());
}


bool write(fat::Entry &entry, const FATInfo &info, const std::vector<uint8_t>& data)
{
    // allocate new clusters
    if (data.size() > entry.size)
    {
        size_t needed_clusters = (data.size() - entry.size) / (info.bootsector.sectors_per_cluster * info.bootsector.bytes_per_sector) + 1;
        auto clusters = find_free_clusters(info, needed_clusters);

        add_clusters(info, entry, clusters);
    }
    else if (clusters(info, data.size()) < get_cluster_chain(entry.low_cluster_bits | (entry.high_cluster_bits << 16), info).size())
    {
        // free unneeded clusters
        const auto cluster_chain = get_cluster_chain(entry.low_cluster_bits | (entry.high_cluster_bits << 16), info);
        const size_t first_cluster_to_free = cluster_chain[clusters(info, data.size())];
        free_cluster_chain(info, first_cluster_to_free);
    }

    auto clusters = get_cluster_chain(entry.low_cluster_bits | (entry.high_cluster_bits << 16), info);
    auto data_chunks = split(data, info.bootsector.sectors_per_cluster * info.bootsector.bytes_per_sector, true);

    for (size_t i { 0 }; i < std::min(data_chunks.size(), clusters.size()); ++i)
    {
        if (!write_cluster(info, clusters[i], data_chunks[i]))
        {
            return false;
        }
    }

    entry.size = data.size();

    return true;
}

void write_bs(const FATInfo& info)
{
    BS bootsector = info.bootsector;
    memcpy(bootsector.extended_section, reinterpret_cast<const void*>(&info.ext16), 54);

    std::vector<uint8_t> data(info.bootsector.bytes_per_sector);
    if (!DiskInterface::read(info.drive, info.base_sector, 1, data.data()))
    {
        return;
    }

    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&bootsector);
    for (size_t i { 0 }; i < sizeof(bootsector)*sizeof(ptr[0]); ++i)
    {
        data[i] = ptr[i];
    }

    if (!DiskInterface::write(info.drive, info.base_sector, 1, data.data()))
    {
        return;
    }
}

}
}
