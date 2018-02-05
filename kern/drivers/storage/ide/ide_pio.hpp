/*
ide_pio.hpp

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
#ifndef IDE_PIO_HPP
#define IDE_PIO_HPP

#include <stdint.h>

#include <vector.hpp>
#include <utility.hpp>
#include <optional.hpp>

#include "ide_common.hpp"

#include "drivers/storage/disk.hpp"

namespace ide::pio
{

void init();

[[nodiscard]] bool read(BusPort port, DriveType type, uint64_t block, size_t count, uint16_t* buf);
[[nodiscard]] bool write(BusPort port, DriveType type, uint64_t block, size_t count, const uint16_t* buf);

uint8_t error_register(BusPort port);
uint8_t status_register(BusPort port);
DiskException::ErrorType get_error(BusPort port);

std::vector<std::pair<BusPort, DriveType>> scan();

std::optional<identify_data> identify(BusPort port, DriveType type);
void cache_flush(BusPort port, DriveType type);

class Disk : public ::DiskImpl<ide::pio::Disk>
{
public:
    Disk(BusPort port, DriveType type);

    virtual size_t disk_size() const override;
    virtual size_t sector_size() const override;
    virtual std::string drive_name() const override;
    virtual void flush_hardware_cache() override;
    virtual Type media_type() const override { return Disk::HardDrive; }

protected:
    virtual std::vector<uint8_t> read_sector(size_t sector, size_t count) const override;
    virtual void write_sector(size_t sector, const std::vector<uint8_t>& data) override;

private:
    void update_id_data() const;

private:
    BusPort m_port;
    DriveType m_type;
    mutable std::optional<identify_data> m_id_data;
};

namespace detail
{
void common(BusPort port, uint8_t type, uint64_t block, uint16_t count);
bool read_one(BusPort port, DriveType type, uint64_t block, uint16_t* buf);
bool write_one(BusPort port, DriveType type, uint64_t block, const uint16_t* buf);
void poll(BusPort port);
void poll_bsy(BusPort port);
bool flush(BusPort port);
bool error_set(BusPort port);
void clear_error(BusPort port);
}
}

#endif // IDE_PIO_HPP
