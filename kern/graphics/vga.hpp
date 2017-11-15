/*
vga.hpp

Copyright (c) 23 Yann BOUCHER (yann)

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
#ifndef VGA_HPP
#define VGA_HPP

#include <stdint.h>

#include "color.hpp"

#include "utils/logging.hpp"

namespace graphics
{

namespace vga
{

/* Hardware text mode color constants. */
enum color : uint8_t
{
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_YELLOW = 14,
    VGA_COLOR_WHITE = 15,
};

static inline uint8_t entry_color(color fg,  color bg)
{
    return fg | bg << 4;
}

static inline uint16_t entry(uint8_t uc, uint8_t color)
{
    return static_cast<uint16_t>(uc) | static_cast<uint16_t>(color) << 8;
}

static inline color color_to_vga(const Color& color)
{
    switch (color.rgb())
    {
    case 0x0000AA:
        return VGA_COLOR_BLUE;
    case 0x00aa00:
        return VGA_COLOR_GREEN;
    case 0x00aaaa:
        return VGA_COLOR_CYAN;
    case 0xaa0000:
        return VGA_COLOR_RED;
    case 0xaa00aa:
        return VGA_COLOR_MAGENTA;
    case 0xaa5500:
        return VGA_COLOR_BROWN;
    case 0xaaaaaa:
        return VGA_COLOR_LIGHT_GREY;
    case 0x555555:
        return VGA_COLOR_DARK_GREY;
    case 0x5555ff:
        return VGA_COLOR_LIGHT_BLUE;
    case 0x55ff55:
        return VGA_COLOR_LIGHT_GREEN;
    case 0x55ffff:
        return VGA_COLOR_LIGHT_CYAN;
    case 0xff5555:
        return VGA_COLOR_LIGHT_RED;
    case 0xff55ff:
        return VGA_COLOR_LIGHT_MAGENTA;
    case 0xffff55:
        return VGA_COLOR_YELLOW;
    case 0xffffff:
        return VGA_COLOR_WHITE;
    }

    return VGA_COLOR_BLACK;
}

}

}

#endif // VGA_HPP
