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

[[nodiscard]] bool read(uint16_t port, uint8_t type, uint64_t block, size_t count, uint16_t* buf);
[[nodiscard]] bool write(uint16_t port, uint8_t type, uint64_t block, size_t count, const uint16_t* buf);

std::vector<std::pair<BusPort, DriveType>> scan();

class Disk : public IDEDisk
{
public:
    Disk(BusPort port, DriveType type);

    template <typename... Args>
    static Disk& create_disk(Args&&... args)
    {
        return DiskImpl<Disk>::create_disk(std::forward<Args>(args)...);
    }

protected:
    [[nodiscard]]
    virtual kpp::expected<MemBuffer, DiskError> read_sector(size_t sector, size_t count) const override;
    [[nodiscard]]
    virtual kpp::expected<kpp::dummy_t, DiskError> write_sector(size_t sector, gsl::span<const uint8_t> data) override;
};

namespace detail
{
bool read_one(uint16_t port, uint8_t type, uint64_t block, uint16_t* buf);
bool write_one(uint16_t port, uint8_t type, uint64_t block, const uint16_t* buf);
}
}

#endif // IDE_PIO_HPP
