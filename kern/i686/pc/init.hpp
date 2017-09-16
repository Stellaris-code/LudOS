/*
init.hpp

Copyright (c) 13 Yann BOUCHER (yann)

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
#ifndef INIT_HPP
#define INIT_HPP

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
#include "i686/pc/termio.hpp"
#include "i686/pc/ide/ide_pio.hpp"

#include "powermanagement.hpp"

#include "terminal/terminal.hpp"

#include "acpi_init.hpp"

#include "utils/bitops.hpp"

extern "C" multiboot_header mbd;

namespace i686
{
namespace pc
{
inline void init(uint32_t magic, const multiboot_info_t* mbd_info)
{
    serial::debug::init(BDA::com1_port());
    serial::debug::write("Serial COM1 : Booting LudOS v%d...\n", 1);

    multiboot::parse_mem(mbd_info);
    Paging::init();

    uint64_t framebuffer_addr = bit_check(mbd_info->flags, 12) ? mbd_info->framebuffer_addr : 0xB8000;

    serial::debug::write("Framebuffer address : 0x%lx\n", phys(framebuffer_addr));

    static TerminalImpl hwterminal(reinterpret_cast<uint16_t*>(phys(framebuffer_addr)), 80, 25);
    Terminal::impl = &hwterminal;
    Terminal::impl->beep_callback = [](size_t ms){Speaker::beep(ms);};
    Terminal::impl->move_cursor_callback = move_cursor;

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

    Speaker::beep(200);

    SMBIOS::locate();
    SMBIOS::bios_info();
    SMBIOS::cpu_info();

    auto status = acpi_init();
    if (ACPI_FAILURE(status))
    {
        err("ACPI Initialization error ! Message : '%s'\n", AcpiFormatException(status));
    }

    Keyboard::handle_char = [](uint8_t c){Terminal::put_char(c);};
    Keyboard::set_kbdmap(kbdmap_fr);
    Keyboard::init();
}
}
}

#endif // INIT_HPP
