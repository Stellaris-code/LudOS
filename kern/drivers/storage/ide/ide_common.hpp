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

#include <optional.hpp>

#include <ctype.h>

#include "drivers/storage/disk.hpp"

#define ATA_ER_BBK      0x80    // Bad sector
#define ATA_ER_UNC      0x40    // Uncorrectable data
#define ATA_ER_MC       0x20    // No media
#define ATA_ER_IDNF     0x10    // ID mark not found
#define ATA_ER_MCR      0x08    // No media
#define ATA_ER_ABRT     0x04    // Command aborted
#define ATA_ER_TK0NF    0x02    // Track 0 not found
#define ATA_ER_AMNF     0x01    // No address mark

#define ATA_ERR         0x01
#define ATA_DRQ         0x08
#define ATA_BSY         0x80

#define ATA_DATA        0x00
#define ATA_ERROR       0x01
#define ATA_FEATURES    0x01
#define ATA_SECCOUNT    0x02
#define ATA_LBALO       0x03
#define ATA_LBAMID      0x04
#define ATA_LBAHI       0x05
#define ATA_SELECT      0x06
#define ATA_STATUS      0x07
#define ATA_CMD         0x07

#define ATA_ALT_STATUS  0x00
#define ATA_DEVCTRL     0x00

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
    uint16_t unused9[136];
    uint8_t checksum_validity;
    uint8_t checksum;
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

struct ata_device
{
    BusPort port;
    DriveType type;
    uint16_t io_base;
    uint16_t control_base;
};

static constexpr uint32_t ata_read_dma_ex = 0x25;
static constexpr uint32_t ata_write_dma_ex = 0x35;
static constexpr uint32_t ata_identify = 0xEC;
static constexpr uint32_t ata_flush_ext = 0xEA;
static constexpr uint32_t ata_nop = 0x00;

template <typename T>
kpp::string ata_string(T&& arr)
{
    using type = std::remove_reference_t<T>;
    static_assert(std::is_array_v<type>, "Must use const char[] !");

    kpp::string str;
    for (size_t i { 0 }; i < std::extent_v<type>; i+=2)
    {
        str += arr[i + 1];
        str += arr[i];
    }

    // trim_right
    while (!str.empty() && isspace(str.back()))
    {
        str.pop_back();
    }
    return str;
}

uint8_t error_register(const ata_device& dev);
uint8_t status_register(const ata_device& dev);
uint8_t drive_register(const ata_device& dev);
DiskError::Type get_error(const ata_device& dev);
uint8_t poll(const ata_device& dev);
uint8_t poll_bsy(const ata_device& dev);
bool flush(const ata_device& dev);
bool error_set(const ata_device& dev);
void clear_error(const ata_device& dev);
void cache_flush(const ata_device& dev);
void wait_400ns(const ata_device& dev);
void soft_reset(const ata_device& dev);

void select(const ata_device& dev, uint64_t block, uint16_t count);
bool detect(const ata_device& dev);
kpp::optional<identify_data> identify(const ata_device& dev);

class IDEDisk : public ::DiskImpl<IDEDisk>
{
public:
    IDEDisk(const ata_device& dev);

    virtual size_t disk_size() const override;
    virtual size_t sector_size() const override;
    virtual kpp::string drive_name() const override;
    virtual void flush_hardware_cache() override;
    virtual Type media_type() const override { return Disk::HardDrive; }

protected:
    [[nodiscard]]
    virtual kpp::expected<kpp::dummy_t, DiskError> read_sectors(size_t sector, gsl::span<uint8_t> data) const override = 0;
    [[nodiscard]]
    virtual kpp::expected<kpp::dummy_t, DiskError> write_sectors(size_t sector, gsl::span<const uint8_t> data) override = 0;

protected:
    void update_id_data() const;

protected:
    ata_device m_dev;
    mutable kpp::optional<identify_data> m_id_data;
};

}

#endif // IDE_COMMON_HPP
