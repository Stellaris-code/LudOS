/*
fat.cpp

Copyright (c) 16 Yann BOUCHER (yann)

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

#include <string.h>

#include <algorithm.hpp>
#include <vector.hpp>

#include "drivers/diskinterface.hpp"

#include "utils/logging.hpp"
#include "string.hpp"
#include "utils/vecutils.hpp"

fat::FATInfo fat::read_fat_fs(size_t drive, size_t base_sector)
{
    std::vector<uint8_t> first_sector(512);

    DiskInterface::read(drive, base_sector, 1, first_sector.data());

    BS bootsector = *reinterpret_cast<BS*>(first_sector.data());

    FATInfo info;
    info.base_sector = base_sector;
    info.bootsector = bootsector;
    info.ext16 = *reinterpret_cast<extBS_16*>(bootsector.extended_section);
    info.ext32 = *reinterpret_cast<extBS_32*>(bootsector.extended_section);

    info.drive = drive;

    if ((bootsector.bootjmp[0] != 0xEB && bootsector.bootjmp[0] != 0xE9) || bootsector.bytes_per_sector == 0)
    {
        info.valid = false;

        return info;
    }

    info.total_sectors = (bootsector.total_sectors_16 == 0) ? bootsector.total_sectors_32 : bootsector.total_sectors_16;
    info.fat_size = (bootsector.table_size_16 == 0)? info.ext32.table_size_32 : bootsector.table_size_16;
    info.root_dir_sectors = ((bootsector.root_entry_count * 32) + (bootsector.bytes_per_sector - 1)) / bootsector.bytes_per_sector;
    info.first_data_sector = bootsector.reserved_sector_count + (bootsector.table_count * info.fat_size) + info.root_dir_sectors;
    info.first_fat_sector = bootsector.reserved_sector_count;
    info.data_sectors = info.total_sectors - (bootsector.reserved_sector_count + (bootsector.table_count * info.fat_size) + info.root_dir_sectors);
    info.total_clusters = info.data_sectors / bootsector.sectors_per_cluster;

    info.valid = true;

    if (info.total_clusters < 4085)
    {
        info.type = FATType::FAT12;
    }
    else if (info.total_clusters < 65525)
    {
        info.type = FATType::FAT16;
    }
    else if (info.total_clusters < 268435445)
    {
        info.type = FATType::FAT32;
    }
    else
    {
        info.type = FATType::ExFAT;
        info.valid = false;
        err("ExFAT is not supported !\n");

        return info;
    }

    return info;
}

size_t fat::detail::first_sector_of_cluster(size_t cluster, const fat::FATInfo &info)
{
    return ((cluster - 2) * info.bootsector.sectors_per_cluster) + info.first_data_sector;
}

void fat::detail::read_FAT_sector(std::vector<uint8_t> &FAT, size_t sector, size_t drive)
{
    DiskInterface::read(drive, sector, 1, FAT.data());
}

uint32_t fat::detail::next_cluster(size_t cluster, const fat::FATInfo &info)
{
    return FAT_entry(get_FAT(info), info, cluster);
}

vfs::node fat::root_dir(const fat::FATInfo &info)
{
    std::vector<vfs::node> entries;
    if (info.type == FATType::FAT32)
    {
        entries = detail::read_cluster_entries(detail::first_sector_of_cluster(info.ext32.root_cluster, info), info);
    }
    else
    {
        entries = detail::read_cluster_entries(info.first_data_sector - info.root_dir_sectors, info);
    }

    vfs::node root_dir;
    detail::fat_file file;
    file.info = info;
    file.is_root = true;
    file.fat_children = entries;
    root_dir.name = "";
    root_dir.data = std::make_shared<detail::fat_file>(file);
    root_dir->is_dir = true;

    return root_dir;
}

std::vector<vfs::node> fat::detail::read_cluster_entries(size_t first_sector, const fat::FATInfo &info)
{
    std::vector<uint8_t> data(512 * info.bootsector.sectors_per_cluster);
    DiskInterface::read(info.drive, first_sector + info.base_sector, info.bootsector.sectors_per_cluster, data.data());

    std::vector<vfs::node> entries;

    size_t entry_idx = 0;
    Entry* cluster_entries = reinterpret_cast<Entry*>(data.data());

    std::string longname_buf;
    do
    {
        Entry* entry = cluster_entries + entry_idx;

        if (entry->filename[0] == 0xE5)
        {
            continue;
        }

        if (entry->attributes & ATTR_LFN)
        {
            std::string vec;
            // Don't forget we are dealing with UCS2 encoding (16-bit)
            for (size_t i { 0 }; i < sizeof(entry->name1)/2; ++i)
            {
                vec.push_back(entry->name1[i*2]);
            }
            for (size_t i { 0 }; i < sizeof(entry->name2)/2; ++i)
            {
                vec.push_back(entry->name2[i*2]);
            }
            for (size_t i { 0 }; i < sizeof(entry->name3)/2; ++i)
            {
                vec.push_back(entry->name3[i*2]);
            }

            longname_buf = vec + longname_buf;

            continue;
        }
        else
        {
            entries.push_back(detail::entry_to_vfs_node(*entry, info, longname_buf));

            size_t cluster = entry->low_cluster_bits | (entry->high_cluster_bits << 16);
            entries.back().get_file<fat_file>().cluster = cluster;

            longname_buf.clear();

            //log("Cluster offset : %d\n", entry_idx);

            if (entry->attributes & ATTR_DIRECTORY)
            {
                const char* filename = reinterpret_cast<const char*>(entry->filename);

                if (std::string(filename, 8) == ".       ")
                {
                    // Current dir
                }
                else if (std::string(filename, 8) == "..      ")
                {
                    // Parent dir
                }
                else
                {
                    entries.back()->is_dir = true;
                }
            }
        }
    } while ((++entry_idx) * sizeof(Entry) < data.size() && cluster_entries[entry_idx].filename[0] != '\0');

    return entries;
}

std::vector<uint8_t> fat::detail::read_cluster(size_t first_sector, const fat::FATInfo &info)
{
    std::vector<uint8_t> data(512 * info.bootsector.sectors_per_cluster);
    DiskInterface::read(info.drive, first_sector + info.base_sector, info.bootsector.sectors_per_cluster, data.data());

    return data;
}

std::vector<uint8_t> fat::detail::read_cluster_chain(size_t cluster, const fat::FATInfo& info)
{
    uint32_t table_entry = next_cluster(cluster, info);

    std::vector<uint8_t> next_entries;
    if (info.type == FATType::FAT12 && table_entry >= 0xFF7)
    {
        next_entries = {};
    }
    else if (info.type == FATType::FAT16 && table_entry >= 0xFFF7)
    {
        next_entries = {};
    }
    else if (info.type == FATType::FAT32 && table_entry >= 0x0FFFFFF7)
    {
        next_entries = {};
    }
    else if (table_entry == 1) // Spec says to consider it as end of chain marker
    {
        next_entries = {};
    }
    else
    {
        next_entries = read_cluster_chain(table_entry, info);
    }

    return read_cluster(first_sector_of_cluster(cluster, info), info) + next_entries;
}

std::vector<size_t> fat::detail::get_cluster_chain(size_t first_cluster, const fat::FATInfo& info)
{
    uint32_t table_entry = next_cluster(first_cluster, info);

    std::vector<size_t> next_clusters;
    if (info.type == FATType::FAT12 && table_entry >= 0xFF7)
    {
        next_clusters = {};
    }
    else if (info.type == FATType::FAT16 && table_entry >= 0xFFF7)
    {
        next_clusters = {};
    }
    else if (info.type == FATType::FAT32 && table_entry >= 0x0FFFFFF7)
    {
        next_clusters = {};
    }
    else if (table_entry == 1) // Spec says to consider it as end of chain marker
    {
        next_clusters = {};
    }
    else
    {
        next_clusters = get_cluster_chain(table_entry, info);
    }

    return merge({first_cluster}, next_clusters);
}

std::vector<uint8_t> fat::detail::read(const fat::Entry &entry, const FATInfo &info)
{
    return read(entry, info, entry.size);
}

std::vector<uint8_t> fat::detail::read(const fat::Entry &entry, const FATInfo &info, size_t nbytes)
{
    if (!(entry.attributes & ATTR_DIRECTORY))
    {
        std::vector<uint8_t> data;
        data = detail::read_cluster_chain(entry.low_cluster_bits | (entry.high_cluster_bits << 16), info);

        data.resize(std::min<size_t>(entry.size, nbytes));
        return data;
    }
    else
    {
        return {};
    }
}

vfs::node fat::detail::entry_to_vfs_node(const fat::Entry &entry, const FATInfo& info, const std::string& long_name)
{
    vfs::node node;
    fat_file file;
    if (!long_name.empty())
    {
        node.name = trim(long_name);
    }
    else
    {
        for (size_t i { 0 }; i < strlen(reinterpret_cast<const char*>(entry.filename)); ++i)
        {
            node.name.push_back(entry.filename[i]);
        }
        node.name = trim(node.name);
        for (size_t i { 0 }; i < strlen(reinterpret_cast<const char*>(entry.extension)); ++i)
        {
            node.name.push_back(entry.extension[i]);
        }
        node.name = trim(node.name);
        std::reverse(node.name.begin(), node.name.end());
    }

    file.flags = entry.attributes;
    file.length = entry.size;
    file.info = info;
    file.entry = entry;
    file.is_dir = false;

    node.data = std::make_unique<fat_file>(file);

    return node;
}

std::vector<uint32_t> fat::detail::find_free_clusters(const FATInfo &info, size_t clusters)
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

std::vector<uint8_t> fat::detail::get_FAT(const FATInfo &info)
{
    std::vector<uint8_t> FAT(info.fat_size*info.bootsector.bytes_per_sector);

    read_FAT_sector(FAT, info.first_fat_sector + info.base_sector, info.drive);

    return FAT;
}

uint32_t fat::detail::FAT_entry(const std::vector<uint8_t>& FAT, const FATInfo &info, size_t cluster)
{
    uint32_t fat_offset = 0;
    if (info.type == FATType::FAT12)
    {
        fat_offset = cluster + cluster / 2;
    }
    else if (info.type == FATType::FAT16)
    {
        fat_offset = cluster * 2;
    }
    else if (info.type == FATType::FAT32)
    {
        fat_offset = cluster * 4;
    }
    uint32_t ent_offset = fat_offset % info.bootsector.bytes_per_sector;
    uint32_t table_value = *reinterpret_cast<const uint32_t*>(&FAT[ent_offset]) & 0x0FFFFFFF;

    if (info.type == FATType::FAT12)
    {
        if(cluster & 0x0001)
            table_value = table_value >> 4;
        else
            table_value = table_value & 0x0FFF;
    }

    return table_value;
}
