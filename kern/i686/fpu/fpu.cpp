/*
fpu.cpp

Copyright (c) 27 Yann BOUCHER (yann)

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

#include "fpu.hpp"

#include "panic.hpp"
#include "../cpu/cpuid.hpp"
#include "utils/bitops.hpp"

#include <assert.h>
#include <string.h>

#include "utils/logging.hpp"
#include "utils/align.hpp"

void FPU::init()
{
    if (!check_cpuid() && !check_fpu_presence())
    {
        log_serial("No FPU found, aborting\n");
        return;
    }

    setup_fpu();
}

alignas(16) uint8_t data[512];

FPUState FPU::save()
{
    assert(((uintptr_t)data & 0xF) == 0);

    asm volatile ("fxsave %0"::"m"(data):"memory");

    FPUState state;
    memcpy(state.data, data, 512);

    return state;
}

void FPU::load(const FPUState &state)
{
    assert(((uintptr_t)data & 0xF) == 0);
    memcpy(data, state.data, 512);

    asm volatile ("fxrstor %0"::"m"(data):"memory");
}

bool FPU::check_cpuid()
{
    unsigned long edx, unused;
    cpuid(1, unused, unused, unused, edx);

    return bit_check(edx, 0);
}
