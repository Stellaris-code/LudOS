/*
termio.hpp

Copyright (c) 13 Yann BOUCHER (yann)

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
#ifndef TERMIO_HPP
#define TERMIO_HPP

#include <stdint.h>

#include "io.hpp"
#include "i686/pc/bios/bda.hpp"

inline void termio_move_cursor(size_t x, size_t y, size_t width)
{

    const size_t index = y * width + x;

    const uint16_t port_low = BDA::video_io_port();
    const uint16_t port_high = port_low + 1;

    // cursor LOW port to vga INDEX register
    outb(port_low, 0x0F);
    outb(port_high, static_cast<uint8_t>(index&0xFF));

    // cursor HIGH port to vga INDEX register
    outb(port_low, 0x0E);
    outb(port_high, static_cast<uint8_t>((index>>8)&0xFF));

}

#endif // TERMIO_HPP
