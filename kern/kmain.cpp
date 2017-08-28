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

// TODO : Paging
// TODO : revoir terminal pour utiliser un init()
// TODO : logs
// TODO : investigate smbios

#ifndef __cplusplus
#error Must be compiler using C++ !
#endif

#include "multiboot/multiboot_kern.hpp"

#include "i686/pc/gdt.hpp"
#include "i686/pc/devices/pic.hpp"
#include "i686/pc/idt.hpp"
#include "i686/pc/devices/pit.hpp"
#include "i686/pc/devices/speaker.hpp"
#include "i686/pc/devices/keyboard.hpp"
#include "i686/pc/fpu.hpp"
#include "i686/pc/cpuinfo.hpp"
#include "i686/pc/cpuid.hpp"

#include "timer.hpp"

#include "utils/logging.hpp"
#include "greet.hpp"
#include "halt.hpp"

extern "C" multiboot_header mbd;

extern "C"
void kmain(uint32_t magic, const multiboot_info_t* mbd_info)
{
    init_printf(nullptr, [](void*, char c){putchar(c);});

    multiboot::check(magic, mbd, mbd_info);
    gdt::init();
    pic::init();
    idt::init();
    PIT::init(100);
    FPU::init();

    log("CPU clock speed : ~%lu MHz\n", clock_speed());
    log("Uptime : %fms\n", uptime());
    detect_cpu();

    multiboot::parse_info(mbd_info);

    Keyboard::init();
    Keyboard::handle_char = &Terminal::put_char;
    Keyboard::set_kbdmap(kbdmap_fr);
    Speaker::beep(200);

    greet();

    while (1)
    {
        NOP();
    }
}
