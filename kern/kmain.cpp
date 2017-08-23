/*
kmain.cpp

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

#include "terminal/terminal.hpp"

#include "utils/multiboot2.h"

#ifndef __cplusplus
#error Must be compiler using C++ !
#endif

extern "C" uint32_t magic;
extern "C" void *mbd;

extern "C"
{
void kmain(void)
{

    if (magic != 0xE85250D6)
    {
        // FIXME : Multiboot error
    }
    Terminal::write_string("Welcome to : \n");

    Terminal::set_color(VGA_COLOR_LIGHT_CYAN);

    Terminal::write_string(R"(   __           _   ___  __ )""\n"
                           R"(  / / _   _  __| | /___\/ _\)""\n"
                           R"( / / | | | |/ _` |//  //\ \ )""\n"
                           R"(/ /__| |_| | (_| / \_// _\ \)""\n"
                           R"(\____/\__,_|\__,_\___/  \__/)""\n");

    ((uint16_t*)0xB8000)[1 * 80 + 2] = vga_entry(' ', VGA_COLOR_BLUE);
}
}
