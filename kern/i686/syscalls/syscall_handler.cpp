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

extern "C" const registers* __attribute__((force_align_arg_pointer)) syscall_handler(registers* const regs)
{
    auto& process = Process::current();

    process.arch_context->regs = *regs;
    process.arch_context->fpu_state = FPU::save();

    uint32_t ret = ENOSYS;

    auto& table = (regs->int_no == ludos_syscall_int ? ludos_syscall_table :
                                                       linux_syscall_table);
    if (regs->eax >= max_syscalls)
    {
        ret = ENOSYS;
        goto exit;
    }

    ret = table[regs->eax].ptr(regs);

exit:
    FPU::load(process.arch_context->fpu_state);
    regs->eax = ret;

    return regs;
}
