/*
scheduler.cpp

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

#include "tasking/scheduler.hpp"

#include "tasking/process_data.hpp"

#include "i686/cpu/registers.hpp"
#include "i686/tasking/process.hpp"

extern "C" registers kernel_yield_regs = {};

static uintptr_t eip;
static uintptr_t esp;

extern "C" [[noreturn]] void do_switch_same(const registers* regs);

extern "C"
void do_kernel_yield()
{
    //    esp = (uintptr_t)__builtin_frame_address(0);
    //    eip = (uintptr_t)__builtin_extract_return_addr(__builtin_return_address(0));

    //    kernel_yield_regs.eip = eip;
    //    kernel_yield_regs.esp = esp + sizeof(uintptr_t)*2; // minus the return address and ebp

    Process::current().arch_context->regs = kernel_yield_regs;
    Process::current().data->kernel_stack_ptr = kernel_yield_regs.esp;

    tasking::schedule();
}
