/*
msr.hpp

Copyright (c) 03 Yann BOUCHER (yann)

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
#ifndef MSR_HPP
#define MSR_HPP

#include <stdint.h>

#include "cpuid.hpp"

inline bool has_msrs()
{
    uint32_t edx, unused;
    cpuid(1, unused, unused, unused, edx);

    return edx & (1 << 5);
}

inline uint64_t read_msr(uint32_t msr_id)
{
    uint32_t low;
    uint32_t high;
    __asm__ __volatile__ ("rdmsr" : "=a"(low), "=d"(high) : "c"(msr_id));
    return (uint64_t) low << 0 | (uint64_t) high << 32;
}

inline void write_msr(uint32_t msr_id, uint64_t msr_value)
{
    uint32_t edx = msr_value >> 32;
    uint32_t eax = msr_value & 0xffffffff;
    __asm__ __volatile__("wrmsr" :: "c"(msr_id), "d"(edx), "a"(eax));
}

#endif // MSR_HPP
