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

// TODO : Passer ca en une classe, FATFs
// TODO : AHCI fait n'importe quoi, vérifier la sortie en IDE et en AHCI comparer

fat::FATInfo fat::read_fat_fs(size_t drive, size_t base_sector, bool read_only)
{
    std::vector<uint8_t> first_sector(512);

    if (!DiskInterface::read(drive, base_sector, 1, first_sector.data()))
    {
        FATInfo info;
        info.valid = false;

        return info;
    }

    BS bootsector = *reinterpret_cast<BS*>(first_sector.data());

    FATInfo info;
    info.base_sector = base_sector;
    info.bootsector = bootsector;
    info.ext16 = *reinterpret_cast<extBS_16*>(bootsector.extended_section);
    info.ext32 = *reinterpret_cast<extBS_32*>(bootsector.extended_section);

    info.drive = drive;
    info.read_only = read_only;

    if ((bootsector.bootjmp[0] != 0xEB && bootsector.bootjmp[0] != 0xE9) || bootsector.bytes_per_sector == 0 || bootsector.sectors_per_cluster == 0)
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
        info.read_only = true;
        if (!read_only) warn("FAT12 is only supported in read-only !\n");
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

    if (!info.read_only) detail::set_dirty_bit(info, true);

    return info;
}

size_t fat::detail::first_sector_of_cluster(size_t cluster, const fat::FATInfo &info)
{
    return ((cluster - 2) * info.bootsector.sectors_per_cluster) + info.first_data_sector;
}

size_t fat::detail::sector_to_cluster(size_t first_sector, const fat::FATInfo &info)
{
    return ((first_sector + 2) * info.bootsector.sectors_per_cluster - info.first_data_sector) / info.bootsector.sectors_per_cluster;
}

uint32_t fat::detail::next_cluster(size_t cluster, const fat::FATInfo &info)
{
    return FAT_entry(info, cluster);
}

fat::fat_file fat::root_dir(const fat::FATInfo &info)
{
    size_t first_sector;

    if (info.type == FATType::FAT32)
    {
        first_sector = detail::first_sector_of_cluster(info.ext32.root_cluster, info);
    }
    else
    {
        first_sector = info.first_data_sector - info.root_dir_sectors;
    }

    fat_file root_dir { nullptr };
    root_dir.info = info;
    root_dir.is_root = true;
    root_dir.entry_first_sector = first_sector;
    root_dir.m_name = "";
    root_dir.m_is_dir = true;
    root_dir.fat_children = detail::read_cluster_entries(first_sector, &root_dir, info);

    return root_dir;
}

std::vector<fat::fat_file> fat::detail::read_cluster_entries(size_t first_sector, fat_file* parent, const fat::FATInfo &info)
{
    std::vector<uint8_t> data(info.bootsector.bytes_per_sector * info.bootsector.sectors_per_cluster);
    if (!DiskInterface::read(info.drive, first_sector + info.base_sector, info.bootsector.sectors_per_cluster, data.data()))
    {
        return {};
    }

    std::vector<fat_file> entries;

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
            const char* filename = reinterpret_cast<const char*>(entry->filename);
            if (entry->attributes & ATTR_DIRECTORY)
            {
                if (std::string(filename, 8) == ".       " ||
                        std::string(filename, 8) == "..      ")
                {
                    continue;
                }
            }

            entries.push_back(detail::entry_to_vfs_node(*entry, parent, info, longname_buf));
            entries.back().entry_first_sector = first_sector;
            entries.back().entry_idx = entry_idx;

            longname_buf.clear();

            if (entry->attributes & ATTR_DIRECTORY)
            {
                size_t cluster = entry->low_cluster_bits | (entry->high_cluster_bits << 16);
                entries.back().dir_cluster = cluster;

                entries.back().m_is_dir = true;
            }
        }
    } while ((++entry_idx) * sizeof(Entry) < data.size() && cluster_entries[entry_idx].filename[0] != '\0');

    return entries;
}

std::vector<uint8_t> fat::detail::read_cluster(size_t first_sector, const fat::FATInfo &info)
{
    std::vector<uint8_t> data(512 * info.bootsector.sectors_per_cluster);
    if (!DiskInterface::read(info.drive, first_sector + info.base_sector, info.bootsector.sectors_per_cluster, data.data()))
    {
        return {};
    }

    return data;
}

std::vector<uint8_t> fat::detail::read_cluster_chain(size_t cluster, const fat::FATInfo& info)
{
    std::vector<uint8_t> data;
    do
    {
        data = merge(data, read_cluster(first_sector_of_cluster(cluster, info), info));

        cluster = next_cluster(cluster, info);
    } while (cluster < end_of_chain(info));


    return data;
}

std::vector<size_t> fat::detail::get_cluster_chain(size_t cluster, const fat::FATInfo& info)
{

    std::vector<size_t> clusters;
    do
    {
        cluster = next_cluster(cluster, info);

        clusters.emplace_back(cluster);
    } while (cluster < end_of_chain(info) && cluster != 1);

    return clusters;
}

std::vector<uint8_t> fat::detail::read(const fat::Entry &entry, const FATInfo &info)
{
    return read(entry, info, entry.size);
}

std::vector<uint8_t> fat::detail::read(const fat::Entry &entry, const FATInfo &info, size_t nbytes)
{
    if (!(entry.attributes & ATTR_DIRECTORY))
    {
        auto chain = detail::get_cluster_chain(entry.low_cluster_bits | (entry.high_cluster_bits << 16), info);

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

fat::fat_file fat::detail::entry_to_vfs_node(const fat::Entry &entry, fat_file* parent, const FATInfo& info, const std::string& long_name)
{
    fat_file node { parent };
    if (!long_name.empty())
    {
        node.m_name = trim(long_name);
    }
    else
    {
        for (size_t i { 0 }; i < strlen(reinterpret_cast<const char*>(entry.filename)); ++i)
        {
            node.m_name.push_back(entry.filename[i]);
        }
        node.m_name = trim(node.m_name);
        for (size_t i { 0 }; i < strlen(reinterpret_cast<const char*>(entry.extension)); ++i)
        {
            node.m_name.push_back(entry.extension[i]);
        }
        node.m_name = trim(node.m_name);
        std::reverse(node.m_name.begin(), node.m_name.end());
    }

    node.set_flags(entry.attributes);
    node.info = info;
    node.entry = entry;
    node.m_is_dir = false;

    return node;
}

std::vector<uint8_t> fat::detail::get_FAT(const FATInfo &info)
{
    std::vector<uint8_t> FAT(info.fat_size*info.bootsector.bytes_per_sector);

    if (!DiskInterface::read(info.drive, info.first_fat_sector + info.base_sector, info.fat_size, FAT.data()))
    {
        return {};
    }

    return FAT;
}

uint32_t fat::detail::FAT_entry(const FATInfo &info, size_t cluster)
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

    unsigned int fat_sector = info.first_fat_sector + (fat_offset / 512);

    uint32_t ent_offset = fat_offset%info.bootsector.bytes_per_sector;

    std::vector<uint8_t> FAT(info.bootsector.bytes_per_sector);

    if (!DiskInterface::read(info.drive, info.base_sector+fat_sector, 1, FAT.data()))
    {
        err("Cannot read FAT from disk %d sector %d\n", info.drive, info.base_sector+fat_sector);
        return end_of_chain(info);
    }

    if (ent_offset > FAT.size())
    {
        warn("FAT entry offset too large : 0x%x\n", ent_offset);
        return end_of_chain(info);
    }

    uint32_t table_value = *reinterpret_cast<const uint16_t*>(&FAT[ent_offset]);
    if (info.type == FATType::FAT32)
    {
        table_value = *reinterpret_cast<const uint32_t*>(&FAT[ent_offset]) & 0x0FFFFFFF;
    }

    if (info.type == FATType::FAT12)
    {
        if(cluster & 0x0001)
            table_value = table_value >> 4;
        else
            table_value = table_value & 0x0FFF;
    }

    return table_value;
}

void fat::detail::set_FAT_entry(const FATInfo &info, size_t cluster, size_t value)
{
    uint32_t fat_offset = 0;

    if (info.type == FATType::FAT16)
    {
        fat_offset = cluster * 2;
    }
    else if (info.type == FATType::FAT32)
    {
        fat_offset = cluster * 4;
    }

    unsigned int fat_sector = info.first_fat_sector + (fat_offset / 512);

    uint32_t ent_offset = fat_offset%info.bootsector.bytes_per_sector;

    std::vector<uint8_t> FAT(info.bootsector.bytes_per_sector);

    if (!DiskInterface::read(info.drive, info.base_sector+fat_sector, 1, FAT.data()))
    {
        warn("Cannot read FAT from disk\n");
        return;
    }

    if (ent_offset > FAT.size())
    {
        warn("FAT entry offset too large : 0x%x\n", ent_offset);
        return;
    }

    if (info.type == FATType::FAT16)
    {
        *reinterpret_cast<uint16_t*>(&FAT[ent_offset]) = value;
    }
    else if (info.type == FATType::FAT32)
    {
        *reinterpret_cast<uint32_t*>(&FAT[ent_offset]) = value & 0x0FFFFFFF;
    }

    if (!info.read_only)
    {
        if (!DiskInterface::write(info.drive, info.base_sector+fat_sector, 1, FAT.data()))
        {
            warn("Cannot write FAT to disk\n");
            return;
        }
    }
}

void fat::unmount(const FATInfo& fs)
{
    if (!fs.read_only) detail::set_dirty_bit(fs, false);
}

size_t fat::detail::end_of_chain(const FATInfo &info)
{
    if (info.type == FATType::FAT12)
    {
        return 0xFF7;
    }
    else if (info.type == FATType::FAT16)
    {
        return 0xFFF7;
    }
    else if (info.type == FATType::FAT32)
    {
        return 0x0FFFFFF7;
    }
    else
    {
        return 0x0FFFFFF7;
    }
}
