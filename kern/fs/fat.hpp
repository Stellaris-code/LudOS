/*
fat.hpp

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
#ifndef FAT_HPP
#define FAT_HPP

#include <stdint.h>

#include <vector.hpp>
#include "string.hpp"

#include "vfs.hpp"

#define ATTR_READ_ONLY 0x01
#define ATTR_HIDDEN 0x02
#define ATTR_SYSTEM 0x04
#define ATTR_VOLUME_ID 0x08
#define ATTR_DIRECTORY 0x10
#define ATTR_ARCHIVE 0x20
#define ATTR_LFN (ATTR_READ_ONLY|ATTR_HIDDEN|ATTR_SYSTEM|ATTR_VOLUME_ID)

namespace fat
{

union Entry
{
    struct
    {
    uint8_t filename[8];
    uint8_t extension[3];

    uint8_t attributes;
    uint8_t nt_reserved;

    uint8_t creation_time_ts;

    uint8_t hour : 5;
    uint8_t min : 6;
    uint8_t sec : 5;

    uint8_t year : 7;
    uint8_t month : 4;
    uint8_t day : 5;

    uint8_t last_access_year : 7;
    uint8_t last_access_month : 4;
    uint8_t last_access_day : 5;

    uint16_t high_cluster_bits;

    uint8_t last_modif_hour : 5;
    uint8_t last_modif_min : 6;
    uint8_t last_modif_sec : 5;

    uint8_t last_modif_year : 7;
    uint8_t last_modif_month : 4;
    uint8_t last_modif_day : 5;

    uint16_t low_cluster_bits;

    uint32_t size;
    }  __attribute__((packed));
    struct
    {
        uint8_t ord;
        uint8_t name1[10];
        uint8_t attr;
        uint8_t type;
        uint8_t chksum;
        uint8_t name2[12];
        uint16_t fstcluslo;
        uint8_t name3[4];
    }  __attribute__((packed));

};

static_assert(sizeof(Entry) == 32);


enum class FATType
{
    FAT12 = 12,
    FAT16 = 16,
    FAT32 = 32,
    ExFAT = 64
};

struct BS
{
    uint8_t 		bootjmp[3];
    uint8_t 		oem_name[8];
    uint16_t 	        bytes_per_sector;
    uint8_t		sectors_per_cluster;
    uint16_t		reserved_sector_count;
    uint8_t		table_count;
    uint16_t		root_entry_count;
    uint16_t		total_sectors_16;
    uint8_t		media_type;
    uint16_t		table_size_16;
    uint16_t		sectors_per_track;
    uint16_t		head_side_count;
    uint32_t 		hidden_sector_count;
    uint32_t 		total_sectors_32;

    //this will be cast to it's specific type once the driver actually knows what type of FAT this is.
    uint8_t		extended_section[54];

} __attribute__((packed));

struct extBS_32
{
    //extended fat32 stuff
    uint32_t		table_size_32;
    uint16_t		extended_flags;
    uint16_t		fat_version;
    uint32_t		root_cluster;
    uint16_t		fat_info;
    uint16_t		backup_BS_sector;
    uint8_t 		reserved_0[12];
    uint8_t		drive_number;
    uint8_t 		reserved_1;
    uint8_t		boot_signature;
    uint32_t 		volume_id;
    uint8_t		volume_label[11];
    uint8_t		fat_type_label[8];

} __attribute__((packed));

struct extBS_16
{
    //extended fat12 and fat16 stuff
    uint8_t		bios_drive_num;
    uint8_t		reserved1;
    uint8_t		boot_signature;
    uint32_t		volume_id;
    uint8_t		volume_label[11];
    uint8_t		fat_type_label[8];

} __attribute__((packed));

struct FATInfo
{
    bool valid;
    size_t base_sector;
    BS bootsector;
    extBS_16 ext16;
    extBS_32 ext32;
    FATType type;
    size_t drive;
    size_t total_sectors;
    size_t fat_size;
    size_t root_dir_sectors;
    size_t first_data_sector;
    size_t first_fat_sector;
    size_t data_sectors;
    size_t total_clusters;
};

namespace detail
{

void read_FAT_sector(std::vector<uint8_t>& FAT, size_t sector, size_t drive);

size_t first_sector_of_cluster(size_t cluster, const FATInfo& info);

uint32_t next_cluster(size_t cluster, const FATInfo& info);

std::vector<vfs::node> read_cluster_entries(size_t first_sector, const FATInfo& info);

std::vector<uint8_t> read_cluster_chain(size_t cluster, const FATInfo& info);

std::vector<uint8_t> read_cluster(size_t first_sector, const fat::FATInfo &info);

vfs::node entry_to_vfs_node(const Entry& entry, const FATInfo &info, const std::string &long_name);
}

FATInfo read_fat_fs(size_t drive, size_t base_sector);

std::vector<vfs::node> root_entries(const FATInfo& info);

std::vector<uint8_t> read(const Entry& entry, const fat::FATInfo& info);
std::vector<uint8_t> read(const Entry& entry, const fat::FATInfo& info, size_t nbytes);

}

#endif // FAT_HPP
