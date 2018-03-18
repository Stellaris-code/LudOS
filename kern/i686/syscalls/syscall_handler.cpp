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

extern "C" uint32_t syscall_handler(const registers* const regs)
{
    Process::current().arch_data->reg_frame = *regs;

    if (regs->eax >= max_syscalls)
    {
        return ENOSYS;
    }

    switch (regs->int_no)
    {
        case ludos_syscall_int:
            return ludos_syscall_table[regs->eax].ptr(regs);
            break;

        case linux_syscall_int:
            return linux_syscall_table[regs->eax].ptr(regs);
            break;

        default:
            err("Invalid syscall interrupt number : 0x%x, pid %d\n", regs->int_no, Process::current().id);
            return ENOSYS;
            break;
    }

    return ENOSYS;
}
