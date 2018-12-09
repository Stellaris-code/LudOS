/*
process.hpp

Copyright (c) 13 Yann BOUCHER (yann)

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
#ifndef i686_PROCESS_HPP
#define i686_PROCESS_HPP

#include "tasking/process.hpp"

#include "i686/fpu/fpu.hpp"
#include "i686/cpu/registers.hpp"

struct ProcessInitRegs
{
    uint32_t edi; // 0x00
    uint32_t esi; // 0x04
    uint32_t ebp; // 0x08
    uint32_t dummy_esp; // 0x0c
    uint32_t ebx; // 0x10
    uint32_t edx; // 0x14
    uint32_t ecx; // 0x18
    uint32_t eax; // 0x1c

    uint32_t eflags; // 0x20

    uint32_t eip; // 0x24
};

struct ProcessArchContext
{
    union
    {
        uintptr_t        esp;
        ProcessInitRegs* init_regs;
    };
    registers*        user_regs;
    FPUState          fpu_state;
};

#endif // i686_PROCESS_HPP
