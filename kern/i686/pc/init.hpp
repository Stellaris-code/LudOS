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

#include "devices/pic.hpp"
#include "devices/pit.hpp"
#include "devices/speaker.hpp"
#include "devices/ps2keyboard.hpp"
#include "devices/ps2mouse.hpp"
#include "i686/fpu/fpu.hpp"
#include "i686/interrupts/idt.hpp"
#include "i686/interrupts/isr.hpp"
#include "i686/cpu/cpuinfo.hpp"
#include "i686/cpu/mtrr.hpp"
#include "i686/simd/simd.hpp"
#include "i686/cpu/cpuid.hpp"
#include "i686/pc/terminal/textterminal.hpp"
#include "i686/pc/mem/mbmeminfo.hpp"
#include "i686/gdt/gdt.hpp"
#include "i686/video/x86emu_modesetting.hpp"
#include "i686/mem/paging.hpp"
#include "smbios/smbios.hpp"
#include "mem/meminfo.hpp"
#include "bios/bda.hpp"
#include "serial/serialdebug.hpp"
#include "drivers/pci/pci.hpp"
#include "drivers/storage/ide/ide_pio.hpp"
#include "drivers/storage/ide/ide_dma.hpp"
#include "drivers/storage/ahci/ahci.hpp"
#include "drivers/acpi/acpi_init.hpp"
#include "drivers/acpi/powermanagement.hpp"
#include "drivers/driver.hpp"
#include "drivers/pci/pcidriver.hpp"

#include "cpp_runtime/exception_support.hpp"

#include "graphics/vga.hpp"

#include "elf/symbol_table.hpp"

#include "terminal/terminal.hpp"

#include "utils/bitops.hpp"
#include "utils/env.hpp"
#include "utils/virt_machine_detect.hpp"
#include "utils/logging.hpp"
#include "utils/defs.hpp"
#include "utils/memutils.hpp"

#include "drivers/pci/pcidriver.hpp"

extern "C" multiboot_header mbd;
extern "C" int kernel_physical_end;

extern "C" int start_ctors;
extern "C" int end_ctors;

extern "C" int _bss_start;
extern "C" int _bss_end;

#pragma GCC push_options
#pragma GCC target ("no-sse")
inline void early_abort(const char* str)
{
    uint16_t* addr = reinterpret_cast<uint16_t*>(0xB8000 + KERNEL_VIRTUAL_BASE);

    // Clear
    for (size_t i { 0 }; i < 25*80; ++i)
    {
        addr[i] = 0;
    }

    while (*str)
    {
        *addr++ = *str++ | (15 << 8);
    }

    halt();
}

void call_ctors()
{
    uint32_t* ctor = (uint32_t*)&start_ctors;
    while (ctor < (uint32_t*)&end_ctors)
    {
        ((void(*)())*ctor)();
        ++ctor;
    }
}

void early_init()
{
    if (simd_features() & SSE)
    {
        enable_sse();
    }
    else
    {
#ifdef __SSE__
        early_abort("LudOS was compiled with SSE support but CPU doesn't support it, aborting");
#endif
    }
}
#pragma GCC pop_options

namespace i686
{
namespace pc
{
inline void init(uint32_t magic, const multiboot_info_t* mbd_info)
{
    early_init();

    call_ctors(); // it is now safe to call global constructors

    serial::debug::init(BDA::com1_port());
    serial::debug::write("Serial COM1 : Booting LudOS v%d...\n", 1);

    gdt::init();

    pic::init();

    idt::init();

    Paging::init();

    log_serial("bob : %d\n", BDA::video_io_port());

    multiboot::check(magic, mbd, mbd_info);
    multiboot::info = mbd_info;

    multiboot::parse_mem();
    MultibootMeminfo::init_alloc_bitmap();

    uint64_t framebuffer_addr = bit_check(mbd_info->flags, 12) ? mbd_info->framebuffer_addr : 0xB8000;

    serial::debug::write("Framebuffer address : 0x%x\n", framebuffer_addr);

    init_printf(nullptr, [](void*, char c){putchar(c);});

    auto lfb = (uint8_t*)Memory::mmap((void*)framebuffer_addr, 80*25*sizeof(uint16_t));

    create_term<TextTerminal>((uintptr_t)lfb, 80, 25, term_data());

    term().clear();

    PIT::init(100);
    Timer::init();

    auto elf_info = multiboot::elf_info();
    elf::kernel_symbol_table = elf::get_symbol_table(elf_info.first, elf_info.second);

    init_exceptions();

    term().set_title(U"LudOS " LUDOS_VERSION_STRING " - build date " __DATE__,
    {0x000000, 0x00aaaa});

    isr::register_handler(isr::Breakpoint, [](const registers* const)
    {
        asm volatile("xchg %bx, %bx");
        return true;
    });

    FPU::init();
    if (simd_features() & SSE)
    {
        log(Debug, "CPU is SSE capable\n");

        aligned_memcpy = _memcpy_mmx;
        if (simd_features() & SSE2)
        {
            aligned_memcpy = _aligned_memcpy_sse2;
            aligned_memsetl = _aligned_memsetl_sse2;
            aligned_double_memsetl = _aligned_double_memsetl_sse2;
        }
    }
    else
    {
#ifdef __SSE__
        panic("No SSE support on this CPU\nThis kernel was built with SSE support, halting\n");
#endif
    }

    log(Info, "End : %p\n", &kernel_physical_end);

    read_from_cmdline(multiboot::parse_cmdline());
    read_logging_config();

    multiboot::print_info();

    SMBIOS::locate();
    SMBIOS::bios_info();
    SMBIOS::cpu_info();

    multiboot::parse_info();

    log(Info, "CPU clock speed : ~%llu MHz\n", clock_speed());
    detect_cpu();

    if (mtrr::available() && mtrr::available_variable_ranges()>0)
    {
        log(Info, "MTRRs available\n");
        mtrr::set_mtrrs_enabled(true);
        mtrr::set_fixed_mtrrs_enabled(false);
    }

    init_emu_mem();

    auto status = acpi_init();
    if (ACPI_FAILURE(status))
    {
        err("ACPI Initialization error ! Message : '%s'\n", AcpiFormatException(status));
        err("\n\n");
        halt();
    }

    acpi::power::init();

    pci::scan();

    Driver::interface_init();
    PciDriver::interface_init();

    if (Driver::get_drivers<ide::dma::Controller>().empty() && !ahci::init())
    {
        ide::pio::init();
    }
}
}
}

#endif // INIT_HPP
