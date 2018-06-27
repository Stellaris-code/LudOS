/*
serialdebug.cpp

Copyright (c) 10 Yann BOUCHER (yann)

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

#include "serialdebug.hpp"

#include <type_traits.hpp>

#include "i686/pc/bios/bda.hpp"
#include "io.hpp"

#include <stdarg.h>
#include <stdio.h>

namespace serial
{
namespace debug
{

void init(uint16_t comport)
{
    set_interrupt_reg(comport, 0);
    set_divisor_rate(comport, 0xc);
    set_data_length(comport, 7);
    set_parity(comport, None);
    set_stop_bits(comport, false);
    set_fifo(comport, 0b11000111);
}

void write(uint16_t comport, const char *fmt, ...)
{
    char buf[512];

    va_list va;
    va_start(va, fmt);
    kvsnprintf(buf, std::extent_v<decltype(buf)>, fmt, va);
    va_end(va);

    char* str = buf;

    while (*str != '\0')
    {
        write_serial(comport, *str);
        outb(0xe9, *str); // bochs
        ++str;
    }
}

void write(const char *fmt, ...)
{
    char buf[512];

    va_list va;
    va_start(va, fmt);
    kvsnprintf(buf, std::extent_v<decltype(buf)>, fmt, va);
    va_end(va);

    char* str = buf;

    while (*str != '\0')
    {
        write_serial(BDA::com1_port(), *str);
        outb(0xe9, *str); // bochs
        ++str;
    }
}

}
}
