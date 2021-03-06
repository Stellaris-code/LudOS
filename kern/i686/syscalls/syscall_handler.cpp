/*
syscall_stub.cpp

Copyright (c) 11 Yann BOUCHER (yann)

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

#include "i686/syscalls/syscall.hpp"

#include "errno.h"

#include "i686/tasking/process.hpp"

#include "i686/tasking/tss.hpp"

#include "tasking/process_data.hpp"


extern "C" const registers* syscall_handler(registers* const regs)
{
    assert(regs->cs & 0x3); // ensure it was called from user mode

    auto& process = Process::current();

    process.arch_context->fpu_state = FPU::save();
    process.arch_context->user_regs = regs;

    uint32_t ret = ENOSYS;

    auto& table = (regs->int_no == ludos_syscall_int ? ludos_syscall_table :
                                                       linux_syscall_table);
    const uint32_t sys_num = regs->eax;

    if (sys_num >= max_syscalls)
    {
        ret = ENOSYS;
        goto exit;
    }

    ret = table[sys_num].ptr(regs);
exit:

    if (table[sys_num].returns) // eax holds return value
    {
        regs->eax = ret;
    }

    FPU::load(process.arch_context->fpu_state); // FIXME

    tss.esp0 = (uintptr_t)(process.data->kernel_stack + ProcessData::kernel_stack_size);

    return regs;
}
