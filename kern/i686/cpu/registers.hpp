/*
registers.hpp

Copyright (c) 26 Yann BOUCHER (yann)

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software", to deal
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
#ifndef REGISTERS_HPP
#define REGISTERS_HPP

#include <stdint.h>

#include "halt.hpp"
#include <stdio.h>

#include "utils/logging.hpp"

typedef struct __attribute__((packed))
{
    // Pushed by the interrupt request/routine handler
    uint32_t gs;
    uint32_t fs;
    uint32_t es;
    uint32_t ds;

    uint32_t eax;
    uint32_t ecx;
    uint32_t edx;
    uint32_t ebx;
    uint32_t ebp;
    uint32_t esi;
    uint32_t edi;

    // Pushed by ISR handler if available
    uint32_t int_no;
    uint32_t err_code;

    // Pushed by the processor
    uint32_t eip;
    uint32_t cs;
    uint32_t eflags;
    uint32_t esp;
    uint32_t ss;
} registers;

inline uint32_t cr0()
{
    uint32_t out;
    __asm__ __volatile__("mov %%cr0, %[var]" : [var] "=r" (out));
    return out;
}
inline uint32_t cr2()
{
    uint32_t out;
    __asm__ __volatile__("mov %%cr2, %[var]" : [var] "=r" (out));
    return out;
}
inline uint32_t cr3()
{
    uint32_t out;
    __asm__ __volatile__("mov %%cr3, %[var]" : [var] "=r" (out));
    return out;
}
inline uint32_t cr4()
{
    uint32_t out;
    __asm__ __volatile__("mov %%cr4, %[var]" : [var] "=r" (out));
    return out;
}


void dump(const registers* regs);

const registers *get_registers();

#endif // REGISTERS_HPP
