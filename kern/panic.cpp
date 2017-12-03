/*
panic.cpp

Copyright (c) 29 Yann BOUCHER (yann)

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

#include "panic.hpp"

#include <stdio.h>
#include <stdarg.h>

#include "utils/defs.hpp"
#include "utils/demangle.hpp"
#include "utils/nop.hpp"
#include "stack-trace.hpp"
#include "elf/symbol_table.hpp"

#include "terminal/terminal.hpp"
#include "dissasembly.hpp"

#ifdef ARCH_i686
#include "i686/pc/devices/speaker.hpp"
#include "i686/interrupts/interrupts.hpp"
#include "i686/cpu/registers.hpp"
#endif

#include "halt.hpp"

const registers* panic_regs = nullptr;

void print_stack_symbols()
{
    auto trace = trace_stack(0);

    // Discard the first function call to kmain, saves space
    for (size_t i { 0 }; i < trace.size()-1; ++i)
    {
        if (auto fun = elf::kernel_symbol_table.get_function(trace[i]); fun)
        {
            auto symbol = *fun;
            kprintf("#%d   0x%x in %s", i+1, trace[i], demangle(symbol.name).c_str());
        }
        else
        {
            kprintf("#%d    0x%x in ????", i+1, trace[i]);
        }
        if (i < trace.size()-2)
        {
            kprintf("\n");
        }
    }
}

void print_disassembly()
{
    kprintf("Disassembly : \n");
    uint8_t* ip = reinterpret_cast<uint8_t*>(panic_regs->eip);

    const size_t dump_len = 6;

    // Move back
    for (size_t i { 0 }; i < dump_len/2; ++i)
    {
        ip -= get_disasm(ip).len;
    }

    for (size_t i { 0 }; i < dump_len; ++i)
    {
        DisasmInfo info = get_disasm(ip);
        if (ip == reinterpret_cast<uint8_t*>(panic_regs->eip))
        {
            kprintf("->  ");
        }
        else
        {
            kprintf("    ");
        }
        kprintf("%s\n", info.str.c_str());
        ip += info.len;
    }
}

[[noreturn]]
void panic(const char *fmt, ...)
{
    cli();

    if (!panic_regs) panic_regs = get_registers();

    term_data().clear();
    term_data().push_color({0xffffff, 0xaa0000});

    term().clear();
    term().set_title(U"KERNEL PANIC", {0xaa0000, 0xffffff});
    term().disable();

    Speaker::beep(300);

    putc_serial = true;

    puts("\nKERNEL PANIC : ");

    {
        va_list va;
        va_start(va, fmt);
        tfp_format(nullptr, [](void*, char c){putchar(c);}, fmt, va);
        va_end(va);
    }

    kprintf("\n");

    dump(panic_regs);

    kprintf("\n");

    print_disassembly();

    kprintf("\n");

    print_stack_symbols();

    term().enable();
    term().scroll_bottom();
    term().draw();

    int i = 10000;
    while (--i) nop();

    Speaker::stop();

    halt();
}
