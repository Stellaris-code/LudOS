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

#include "gdt/gdt.hpp"
#include "devices/pic.hpp"
#include "interrupts/idt.hpp"
#include "devices/pit.hpp"
#include "devices/speaker.hpp"
#include "devices/ps2keyboard.hpp"
#include "devices/ps2mouse.hpp"
#include "devices/rtc.hpp"
#include "fpu/fpu.hpp"
#include "cpu/cpuinfo.hpp"
#include "mem/meminfo.hpp"
#include "cpu/cpuid.hpp"
#include "smbios/smbios.hpp"
#include "mem/paging.hpp"
#include "bios/bda.hpp"
#include "serial/serialdebug.hpp"
#include "io/termio.hpp"
#include "pci/pci.hpp"
#include "ide/ide_pio.hpp"
#include "ahci/ahci.hpp"
#include "acpi/acpi_init.hpp"

#include "common/defs.hpp"

#include "graphics/vga.hpp"

#include "acpi/powermanagement.hpp"

#include "terminal/terminal.hpp"

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

    static Terminal hwterminal(80, 25, 15);
    term = &hwterminal;
    term->beep_callback = [](size_t ms){Speaker::beep(ms);};
    term->move_cursor_callback = move_cursor;
    term->putchar_callback = [framebuffer_addr](size_t x, size_t y, uint8_t c, TermEntry color)
    {
        auto fb = reinterpret_cast<uint16_t*>(phys(framebuffer_addr));
        fb[y*term->width() + x] = vga_entry(c, vga_entry_color(color_to_vga(color.fg), color_to_vga(color.bg)));
    };

    term->clear();

    init_printf(nullptr, [](void*, char c){putchar(c);});

    multiboot::check(magic, mbd, mbd_info);

    gdt::init();
    pic::init();

    term->set_title("LudOS " LUDOS_VERSION_STRING " - build date " __DATE__);

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

    acpi::power::init();

    rtc::init();

    pci::scan();

    if (!ahci::init())
    {
        ide::pio::init();
    }

    PS2Keyboard::init();
    PS2Mouse::init();
}
}
}

#endif // INIT_HPP
