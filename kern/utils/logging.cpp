/*
logging.cpp

Copyright (c) 09 Yann BOUCHER (yann)

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

#include "logging.hpp"

#include "terminal/terminal.hpp"

#ifdef ARCH_i686
#include "i686/pc/serial/serialdebug.hpp"
#include "i686/pc/time/timestamp.hpp"
#endif

void log(const char * __restrict fmt, ...)
{
    kprintf("[%f] ", uptime());

    va_list va;
    va_start(va, fmt);
    tfp_format(nullptr, [](void*, char c){putchar(c);}, fmt, va);
    va_end(va);
}

void warn(const char * __restrict fmt, ...)
{
    term->push_color({0xff5555, 0});

    kprintf("[%f] ", uptime());

    va_list va;
    va_start(va, fmt);
    tfp_format(nullptr, [](void*, char c){putchar(c); serial::debug::write("%c", c);}, fmt, va);
    va_end(va);

    term->pop_color();
}

void err(const char * __restrict fmt, ...)
{
    term->push_color({0xaa0000, 0});

    kprintf("[%f] ", uptime());

    va_list va;
    va_start(va, fmt);
    tfp_format(nullptr, [](void*, char c){putchar(c); serial::debug::write("%c", c);}, fmt, va);
    va_end(va);

    term->pop_color();
}

void log_serial(const char * __restrict fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    tfp_format(nullptr, [](void*, char c){serial::debug::write("%c", c);}, fmt, va);
    va_end(va);
}
