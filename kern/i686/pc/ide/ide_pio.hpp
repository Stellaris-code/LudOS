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

namespace ide
{

namespace pio
{

void init();

[[nodiscard]] bool read(BusPort port, DriveType type, uint64_t block, size_t count, uint16_t* buf);
[[nodiscard]] bool write(BusPort port, DriveType type, uint64_t block, size_t count, const uint16_t* buf);

uint8_t error_register(BusPort port);
uint8_t status_register(BusPort port);

std::vector<std::pair<BusPort, DriveType>> scan();

std::optional<identify_data> identify(BusPort port, DriveType type);

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
}

#endif // IDE_PIO_HPP
