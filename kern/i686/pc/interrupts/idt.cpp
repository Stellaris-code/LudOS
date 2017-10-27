/*
idt.cpp

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

#include "idt.hpp"

#include <type_traits.hpp>

#include <string.h>
#include <stdio.h>

#include "utils/logging.hpp"

#include "interrupts.hpp"

namespace idt
{

extern "C"
void idt_flush(uint32_t addr);

entry idt_entries[256];
ptr  idt_ptr;

void set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags)
{
    idt_entries[num].base_lo = base & 0xFFFF;
    idt_entries[num].base_hi = (base >> 16) & 0xFFFF;

    idt_entries[num].sel     = sel;
    idt_entries[num].always0 = 0;
    // We must uncomment the OR below when we get to using user-mode.
    // It sets the interrupt gate's privilege level to 3.
    idt_entries[num].flags   = flags /* | 0x60 */;
}

void init()
{
    puts("Initializing ISRs");

    idt_ptr.limit = sizeof(entry) * std::extent_v<decltype(idt_entries)> -1;
    idt_ptr.base  = reinterpret_cast<uint32_t>(&idt_entries);

    memset(&idt_entries, 0, sizeof(entry)*std::extent_v<decltype(idt_entries)>);

    set_gate( 0, reinterpret_cast<uint32_t>(isr0) , 0x08, 0x8E);
    set_gate( 1, reinterpret_cast<uint32_t>(isr1) , 0x08, 0x8E);
    set_gate( 2, reinterpret_cast<uint32_t>(isr2) , 0x08, 0x8E);
    set_gate( 3, reinterpret_cast<uint32_t>(isr3) , 0x08, 0x8E);
    set_gate( 4, reinterpret_cast<uint32_t>(isr4) , 0x08, 0x8E);
    set_gate( 5, reinterpret_cast<uint32_t>(isr5) , 0x08, 0x8E);
    set_gate( 6, reinterpret_cast<uint32_t>(isr6) , 0x08, 0x8E);
    set_gate( 7, reinterpret_cast<uint32_t>(isr7) , 0x08, 0x8E);
    set_gate( 8, reinterpret_cast<uint32_t>(isr8) , 0x08, 0x8E);
    set_gate( 9, reinterpret_cast<uint32_t>(isr9) , 0x08, 0x8E);
    set_gate(10, reinterpret_cast<uint32_t>(isr10), 0x08, 0x8E);
    set_gate(11, reinterpret_cast<uint32_t>(isr11), 0x08, 0x8E);
    set_gate(12, reinterpret_cast<uint32_t>(isr12), 0x08, 0x8E);
    set_gate(13, reinterpret_cast<uint32_t>(isr13), 0x08, 0x8E);
    set_gate(14, reinterpret_cast<uint32_t>(isr14), 0x08, 0x8E);
    set_gate(15, reinterpret_cast<uint32_t>(isr15), 0x08, 0x8E);
    set_gate(16, reinterpret_cast<uint32_t>(isr16), 0x08, 0x8E);
    set_gate(17, reinterpret_cast<uint32_t>(isr17), 0x08, 0x8E);
    set_gate(18, reinterpret_cast<uint32_t>(isr18), 0x08, 0x8E);
    set_gate(19, reinterpret_cast<uint32_t>(isr19), 0x08, 0x8E);
    set_gate(20, reinterpret_cast<uint32_t>(isr20), 0x08, 0x8E);
    set_gate(21, reinterpret_cast<uint32_t>(isr21), 0x08, 0x8E);
    set_gate(22, reinterpret_cast<uint32_t>(isr22), 0x08, 0x8E);
    set_gate(23, reinterpret_cast<uint32_t>(isr23), 0x08, 0x8E);
    set_gate(24, reinterpret_cast<uint32_t>(isr24), 0x08, 0x8E);
    set_gate(25, reinterpret_cast<uint32_t>(isr25), 0x08, 0x8E);
    set_gate(26, reinterpret_cast<uint32_t>(isr26), 0x08, 0x8E);
    set_gate(27, reinterpret_cast<uint32_t>(isr27), 0x08, 0x8E);
    set_gate(28, reinterpret_cast<uint32_t>(isr28), 0x08, 0x8E);
    set_gate(29, reinterpret_cast<uint32_t>(isr29), 0x08, 0x8E);
    set_gate(30, reinterpret_cast<uint32_t>(isr30), 0x08, 0x8E);
    set_gate(31, reinterpret_cast<uint32_t>(isr31), 0x08, 0x8E);
    set_gate(32, reinterpret_cast<uint32_t>(irq0 ), 0x08, 0x8E);
    set_gate(33, reinterpret_cast<uint32_t>(irq1 ), 0x08, 0x8E);
    set_gate(34, reinterpret_cast<uint32_t>(irq2 ), 0x08, 0x8E);
    set_gate(35, reinterpret_cast<uint32_t>(irq3 ), 0x08, 0x8E);
    set_gate(36, reinterpret_cast<uint32_t>(irq4 ), 0x08, 0x8E);
    set_gate(37, reinterpret_cast<uint32_t>(irq5 ), 0x08, 0x8E);
    set_gate(38, reinterpret_cast<uint32_t>(irq6 ), 0x08, 0x8E);
    set_gate(39, reinterpret_cast<uint32_t>(irq7 ), 0x08, 0x8E);
    set_gate(40, reinterpret_cast<uint32_t>(irq8 ), 0x08, 0x8E);
    set_gate(41, reinterpret_cast<uint32_t>(irq9 ), 0x08, 0x8E);
    set_gate(42, reinterpret_cast<uint32_t>(irq10), 0x08, 0x8E);
    set_gate(43, reinterpret_cast<uint32_t>(irq11), 0x08, 0x8E);
    set_gate(44, reinterpret_cast<uint32_t>(irq12), 0x08, 0x8E);
    set_gate(45, reinterpret_cast<uint32_t>(irq13), 0x08, 0x8E);
    set_gate(46, reinterpret_cast<uint32_t>(irq14), 0x08, 0x8E);
    set_gate(47, reinterpret_cast<uint32_t>(irq15), 0x08, 0x8E);

    idt_flush(reinterpret_cast<uint32_t>(&idt_ptr));

    sti();

    puts("ISRs and IDT initialized");
}

}
