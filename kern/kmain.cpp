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

// TODO : APM
// TODO : ACPI
// TODO : disque
// TODO : ext2
// TODO : scrolling terminal char par char
// TODO : system calls
// TODO : user mode
// TODO : POC calculatrice
// TODO : utiliser une vraie implémentation de printf (newlib ?)

// FIXME : bug si clavier utilisé avant init

#ifndef __cplusplus
#error Must be compiler using C++ !
#endif

#ifndef NDEBUG
#define DEBUG
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
#include "i686/pc/smbios.hpp"
#include "i686/pc/paging.hpp"
#include "i686/pc/bios/bda.hpp"
#include "i686/pc/serialdebug.hpp"

#include "utils/addr.hpp"
#include "utils/bitarray.hpp"
#include "utils/dynarray.hpp"

#include "timer.hpp"

#include "ext/liballoc/liballoc.h"

#include "utils/logging.hpp"
#include "greet.hpp"
#include "halt.hpp"

extern "C" multiboot_header mbd;
extern "C" uint32_t kernel_physical_end;

extern "C"
void kmain(uint32_t magic, const multiboot_info_t* mbd_info)
{
    serial::debug::init(BDA::com1_port());
    serial::debug::write(BDA::com1_port(), "Serial COM1 : Booting LudOS v%d...\n", 1);

    TerminalImpl hwterminal(reinterpret_cast<uint16_t*>(phys(0xB8000)), 80, 25);
    Terminal::impl = &hwterminal;

    init_printf(nullptr, [](void*, char c){putchar(c);});

    multiboot::check(magic, mbd, mbd_info);
    gdt::init();
    pic::init();
    idt::init();
    PIT::init(100);
    FPU::init();
    multiboot::parse_info(mbd_info);

    log("CPU clock speed : ~%llu MHz\n", clock_speed());
    detect_cpu();
    log("plop\n");
    // paging fucks up i/o
    Paging::init();


    Speaker::beep(200);



    SMBIOS::locate();
    SMBIOS::bios_info();
    SMBIOS::cpu_info();

    Keyboard::init();
    Keyboard::handle_char = [](uint8_t c){Terminal::put_char(c);};
    Keyboard::set_kbdmap(kbdmap_fr);

    greet();

    char* ptr = (char*)kmalloc(256*1000*1000);
    liballoc_dump();
    ptr[4] = 'c';
    kfree(ptr);
    liballoc_dump();

    while (1)
    {
//        log("dd\n");
        NOP();
    }
}
