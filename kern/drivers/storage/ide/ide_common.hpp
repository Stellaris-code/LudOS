/*
ide_common.hpp

Copyright (c) 07 Yann BOUCHER (yann)

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
#ifndef IDE_COMMON_HPP
#define IDE_COMMON_HPP

#include <stdint.h>

#include <string.hpp>
#include <optional.hpp>

#include "utils/stlutils.hpp"

#include "drivers/storage/disk.hpp"

#define ATA_ER_BBK      0x80    // Bad sector
#define ATA_ER_UNC      0x40    // Uncorrectable data
#define ATA_ER_MC       0x20    // No media
#define ATA_ER_IDNF     0x10    // ID mark not found
#define ATA_ER_MCR      0x08    // No media
#define ATA_ER_ABRT     0x04    // Command aborted
#define ATA_ER_TK0NF    0x02    // Track 0 not found
#define ATA_ER_AMNF     0x01    // No address mark

namespace ide
{

struct [[gnu::packed]] identify_data
{
    uint16_t flags; // 0
    uint16_t unused1[9]; // 9
    char     serial[20]; // 19
    uint16_t unused2[3]; // 22
    char     firmware[8]; // 26
    char    model[40]; // 46
    uint16_t sectors_per_int; // 47
    uint16_t unused3; // 48
    uint16_t capabilities[2]; // 50
    uint16_t unused4[2]; // 52
    uint16_t valid_ext_data; // 53
    uint16_t unused5[5]; // 58
    uint16_t size_of_rw_mult; // 59
    uint32_t sectors_28; // 61
    uint16_t unused6[38]; // 99
    uint64_t sectors_48; // 103
    uint16_t unused7[2]; // 105
    uint16_t phys_log_size; // 106
    uint16_t unused8[10]; // 116
    uint32_t sector_size; // 118
    uint16_t unused9[137];
};

static_assert(sizeof(identify_data) == 512);

enum DriveType : uint8_t
{
    Master = 0xE0,
    Slave = 0xF0
};

enum BusPort : uint16_t
{
    Primary = 0x1F0,
    Secondary = 0x170,
    Third = 0x1E8,
    Fourth = 0x168
};

static constexpr uint32_t ata_read_dma_ex = 0x25;
static constexpr uint32_t ata_write_dma_ex = 0x35;
static constexpr uint32_t ata_identify = 0xEC;
static constexpr uint32_t ata_flush_ext = 0xEA;

template <typename T>
std::string ata_string(T&& arr)
{
    using type = std::remove_reference_t<T>;
    static_assert(std::is_array_v<type>, "Must use const char[] !");

    std::string str;
    for (size_t i { 0 }; i < std::extent_v<type>; i+=2)
    {
        str += arr[i + 1];
        str += arr[i];
    }

    return trim_right(str);
}

uint8_t error_register(uint16_t port);
uint8_t status_register(uint16_t port);
uint8_t drive_register(uint16_t port);
DiskException::ErrorType get_error(uint16_t port);
void poll(uint16_t port);
void poll_bsy(uint16_t port);
bool flush(uint16_t port);
bool error_set(uint16_t port);
void clear_error(uint16_t port);
void cache_flush(uint16_t port, uint8_t type);

void select(uint16_t port, uint8_t type, uint64_t block, uint16_t count);
std::optional<identify_data> identify(uint16_t port, uint8_t type);

class IDEDisk : public ::DiskImpl<IDEDisk>
{
public:
    IDEDisk(uint16_t port, uint8_t type);

    virtual size_t disk_size() const override;
    virtual size_t sector_size() const override;
    virtual std::string drive_name() const override;
    virtual void flush_hardware_cache() override;
    virtual Type media_type() const override { return Disk::HardDrive; }

protected:
    virtual std::vector<uint8_t> read_sector(size_t sector, size_t count) const override = 0;
    virtual void write_sector(size_t sector, const std::vector<uint8_t>& data) override = 0;

protected:
    void update_id_data() const;

protected:
    uint16_t m_port;
    uint8_t m_type;
    mutable std::optional<identify_data> m_id_data;
};

}

#endif // IDE_COMMON_HPP
