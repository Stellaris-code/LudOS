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

#include "panic.hpp"
#include "errno.h"

#include "i686/tasking/process.hpp"
#include "utils/align.hpp"

extern "C" uint32_t __attribute__((force_align_arg_pointer)) syscall_handler(registers* const regs)
{
    processing_syscall = true;

    Process::current().arch_data->regs = *regs;
    Process::current().arch_data->fpu_state = FPU::save();

    uint32_t ret = ENOSYS;

    if (regs->eax >= max_syscalls)
    {
        ret = ENOSYS;
        return ret;
    }

    switch (regs->int_no)
    {
        case ludos_syscall_int:
            ret = ludos_syscall_table[regs->eax].ptr(regs);
            break;

        case linux_syscall_int:
            ret = linux_syscall_table[regs->eax].ptr(regs);
            break;

        default:
            err("Invalid syscall interrupt number : 0x%x, pid %d\n", regs->int_no, Process::current().pid);
            ret = ENOSYS;
            return ret;
    }

    FPU::load(Process::current().arch_data->fpu_state);
    regs->eax = ret;
    processing_syscall = false;
    return ret;
}
