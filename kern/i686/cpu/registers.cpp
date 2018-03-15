/*
registers.cpp

Copyright (c) 24 Yann BOUCHER (yann)

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

#include "registers.hpp"

#include "assert.h"

registers cur_regs;

extern "C"
void update_registers();

const registers *get_registers()
{
    update_registers();
    return &cur_regs;
}

extern "C"
const registers* handle_registers_request(const registers *regs)
{
    cur_regs = *regs;
    return regs;
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat"
void dump(const registers *regs)
{
    if (regs)
    {
        kprintf("eip : 0x%x int : 0x%x  err code : 0x%x\n", regs->eip, regs->int_no, regs->err_code);
        kprintf("eax : 0x%x  ebx : 0x%x  ", regs->eax, regs->ebx);
        kprintf("ecx : 0x%x  edx : 0x%x  \n", regs->ecx, regs->edx);
        kprintf("ebx : 0x%x  esi : 0x%x  ", regs->ebp, regs->esi);
        kprintf("edi : 0x%x  ebp : 0x%x\n", regs->edi, regs->ebp);
        kprintf("cr0 : 0x%x  cr2 : 0x%x cr3 : 0x%x  cr4 : 0x%x\n", cr0(), cr2(), cr3(), cr4());
        kprintf("gs : 0x%x  fs : 0x%x  ", regs->gs, regs->fs);
        kprintf("es : 0x%x  ds : 0x%x\n", regs->es, regs->ds);
        kprintf("cs : 0x%x  eflags : 0x%x  ", regs->cs, regs->eflags);
        kprintf("ss : 0x%x  esp : 0x%x  CPL : %d (%s)\n", regs->ss, regs->esp, regs->cs & 0x3, regs->cs & 0x3 ? "User" : "Kernel");
    }
}
#pragma GCC diagnostic pop
