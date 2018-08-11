/*
pagefaulthandler.cpp

Copyright (c) 17 Yann BOUCHER (yann)

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

#include "paging.hpp"

#include "panic.hpp"

#include "mem/page_fault.hpp"

#include <libdisasm/libdis.h>

bool Paging::page_fault_handler(registers *regs)
{
    panic_regs = regs;

    PageFault fault;
    fault.mcontext = regs;
    fault.address = cr2();
    fault.level = (regs->err_code & (1<<2)) ? PageFault::User : PageFault::Kernel;
    fault.error = (regs->err_code & (1<<0)) ? PageFault::Protection : PageFault::NonPresent;
    fault.type  = (regs->err_code & (1<<1)) ? PageFault::Write :
                                              (regs->err_code & (1<<4)) ? PageFault::Execute
                                                                        : PageFault::Read;

    if (regs->err_code & (1<<3)) // reserved bit write, this should definitely never happen
    {
        panic("Reserved paging structure bit write !\n");
    }

    page_fault_entry(fault);

    // if eip seems invalid, try to manually pop the stack and return
    if (!Memory::is_mapped((unsigned char*)regs->eip))
    {
        uintptr_t return_eip = *(uintptr_t*)(regs->esp);

        regs->eip = return_eip;
        regs->esp += sizeof(uintptr_t);
    }
    else
    {
        // if we actually return, move eip to the next instruction
        x86_invariant_t ins;
        x86_invariant_disasm((unsigned char*)regs->eip, x86_max_insn_size(), &ins);

        regs->eip += ins.size;
    }

    return true;
}
