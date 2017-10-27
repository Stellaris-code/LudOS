/*
color.hpp

Copyright (c) 29 Yann BOUCHER (yann)

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
#ifndef COLOR_HPP
#define COLOR_HPP

#include <stdint.h>

struct Color
{
    union
    {
        struct
        {
            uint8_t b;
            uint8_t g;
            uint8_t r;
        };
        uint32_t rgb;
    };

    Color() = default;
    Color(uint8_t ir, uint8_t ig, uint8_t ib)
        : r(ir), g(ig), b(ib)
    {}
    Color(uint32_t irgb)
        : rgb(irgb)
    {}
};

struct TermEntry
{
    Color fg;
    Color bg;
};

#endif // COLOR_HPP
