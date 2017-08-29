/*
panic.hpp

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
#ifndef PANIC_HPP
#define PANIC_HPP

#include <stdio.h>
#include <stdarg.h>

#include "terminal/terminal.hpp"

#include "i686/pc/devices/speaker.hpp"
#include "i686/pc/interrupts.hpp"

#include "halt.hpp"

[[noreturn]]
inline void panic(const char* __restrict fmt, ...)
{
    Terminal::set_color(VGA_COLOR_RED);

    //Speaker::beep(300);

    cli();

    puts("\nKERNEL PANIC : ");

    va_list va;
    va_start(va, fmt);
    tfp_format(nullptr, [](void*, char c){putchar(c);}, fmt, va);
    va_end(va);
    halt();
}

#endif // PANIC_HPP
