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

// TODO : Beep !
// TODO : Keyboard
// TODO : Paging
// TODO : revoir terminal pour utiliser un init()


#ifndef __cplusplus
#error Must be compiler using C++ !
#endif

#include <stdio.h>

#include "multiboot/multiboot_kern.hpp"

#include "i686/pc/gdt.hpp"
#include "i686/pc/pic.hpp"
#include "i686/pc/idt.hpp"
#include "i686/pc/pit.hpp"
#include "i686/pc/speaker.hpp"

#include "timer.hpp"

#include "greet.hpp"
#include "halt.hpp"

extern "C" multiboot_header mbd;

extern "C"
void kmain(uint32_t magic, const multiboot_info_t* mbd_info)
{
    multiboot::check(magic, mbd, mbd_info);

    gdt::init();
    pic::init();
    idt::init();
    PIT::init(100);

    multiboot::parse_info(mbd_info);

    Speaker::beep(200);

    greet();

    while (1)
    {
        NOP();
    }
}
