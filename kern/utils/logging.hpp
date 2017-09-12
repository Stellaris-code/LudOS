/*
logging.hpp

Copyright (c) 27 Yann BOUCHER (yann)

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
#ifndef LOGGING_HPP
#define LOGGING_HPP

#include <stdio.h>
#include <stdarg.h>

#include "terminal/terminal.hpp"

#include "i686/pc/timestamp.hpp"

inline void log(const char * __restrict fmt, ...)
{
    printf("[%f] ", uptime());

    va_list va;
    va_start(va, fmt);
    tfp_format(nullptr, [](void*, char c){putchar(c);}, fmt, va);
    va_end(va);
}

inline void warn(const char * __restrict fmt, ...)
{
    Terminal::push_color(VGA_COLOR_LIGHT_RED);

    printf("[%f] ", uptime());

    va_list va;
    va_start(va, fmt);
    tfp_format(nullptr, [](void*, char c){putchar(c);}, fmt, va);
    va_end(va);

    Terminal::pop_color();
}

inline void err(const char * __restrict fmt, ...)
{
    Terminal::push_color(VGA_COLOR_RED);

    printf("[%f] ", uptime());

    va_list va;
    va_start(va, fmt);
    tfp_format(nullptr, [](void*, char c){putchar(c);}, fmt, va);
    va_end(va);

    Terminal::pop_color();
}

#endif // LOGGING_HPP
