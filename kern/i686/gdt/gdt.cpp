/*
gdt.cpp

Copyright (c) 26 Yann BOUCHER (yann)

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
#ifndef GDT_CPP
#define GDT_CPP

#include <stdio.h>

#include <type_traits.hpp>

#include "gdt.hpp"
#include "panic.hpp"
#include "../tasking/tss.hpp"
#include "utils/addr.hpp"
#include "utils/logging.hpp"

namespace gdt
{

entry entries[9];
ptr gdt_ptr;

void set_gate(size_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran)
{
    if (num >= sizeof(entries))
    {
        panic("Invalid GDT entry : num is larger than entries");
    }

    entries[num].base_low    = (base & 0xFFFF);
    entries[num].base_middle = (base >> 16) & 0xFF;
    entries[num].base_high   = (base >> 24) & 0xFF;

    entries[num].limit_low   = (limit & 0xFFFF);
    entries[num].granularity = (limit >> 16) & 0x0F;

    entries[num].granularity |= gran & 0xF0;
    entries[num].access      = access;
}

void init()
{
    gdt_ptr.limit = (sizeof(entry) * std::extent_v<decltype(entries)>) - 1;
    gdt_ptr.base  = reinterpret_cast<uint32_t>(&entries);

    tss.trap = 0x00;
    tss.iomap = 0x00;
    tss.esp0 = 0x20000;
    tss.ss0 = 0x18;

    set_gate(0, 0, 0, 0, 0);                // Null segment
    set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF); // Code segment
    set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF); // Data segment
    set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF); // User mode code segment
    set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF); // User mode data segment
    set_gate(5, reinterpret_cast<uint32_t>(&tss), sizeof(tss), 0x89, 0xCF);

    flush();

    log(Info, "GDT initialized.\n");
}

void flush()
{
    gdt_flush(reinterpret_cast<uint32_t>(&gdt_ptr));
}

}

#endif // GDT_CPP

