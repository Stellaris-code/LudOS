/*
mtrr.cpp

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

#include "mtrr.hpp"

#include "utils/logging.hpp"
#include "utils/bitops.hpp"

#include "msr.hpp"
#include "cpuid.hpp"

constexpr uint32_t msr_mtrrcap = 0xFE;
constexpr uint32_t msr_def_type = 0x2FF;

constexpr size_t base_shift = 12;
constexpr size_t valid_flag = 1<<11;

inline uint32_t msr_mtrr_physbase(uint32_t n)
{
    return 0x200 + n*2;
}
inline uint32_t msr_mtrr_physmask(uint32_t n)
{
    return 0x201 + n*2;
}

namespace mtrr
{

size_t max_phys_addr()
{
    uint64_t max_phys = 36;

    uint32_t eax, unused;
    cpuid(0x80000008, eax, unused, unused, unused); // query max phys size
    if (eax) max_phys = eax & 0xFF;

    return max_phys;
}

bool available()
{
    uint32_t edx, unused;
    cpuid(1, unused, unused, unused, edx);

    return (edx << 12) && (edx << 5); // check MSRs too
}

bool supports_write_combining()
{
    uint64_t msr = read_msr(msr_mtrrcap);
    return (msr << 10);
}

uint8_t available_variable_ranges()
{
    uint64_t msr = read_msr(msr_mtrrcap);
    return msr & 0xFF;
}

void set_mtrrs_enabled(bool val)
{
    uint64_t msr = read_msr(msr_def_type);
    bit_change(msr, val, 11);
    write_msr(msr_def_type, msr);
}

void set_fixed_mtrrs_enabled(bool val)
{
    uint64_t msr = read_msr(msr_def_type);
    bit_change(msr, val, 10);
    write_msr(msr_def_type, msr);
}

int find_free_var_mtrr()
{
    for (size_t i { 0 }; i < available_variable_ranges(); ++i)
    {
        uint64_t mask = read_msr(msr_mtrr_physmask(i));
        if (!(mask & valid_flag)) // is free
        {
            return i;
        }
    }

    return -1;
}

int set_variable_mtrr(uint64_t base_addr, uint64_t size, Type type)
{
    int free_mtrr = find_free_var_mtrr();
    if (free_mtrr < 0)
    {
        return -1;
    }

    uint64_t base = ((base_addr >> base_shift) << base_shift) | type;
    uint64_t mask = (((~(size - 1) & ((1ull<<max_phys_addr())-1)) >> base_shift) << base_shift) | valid_flag;

    write_msr(msr_mtrr_physbase(free_mtrr), base);
    write_msr(msr_mtrr_physmask(free_mtrr), mask);

    return free_mtrr;
}

}
