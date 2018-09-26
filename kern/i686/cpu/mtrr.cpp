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

#include <assert.h>

#include "utils/logging.hpp"
#include "utils/bitops.hpp"
#include "utils/env.hpp"

#include "msr.hpp"
#include "i686/interrupts/interrupts.hpp"
#include "registers.hpp"
#include "asmops.hpp"
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

    // next pow of two
    size--;
    size |= size >> 1;
    size |= size >> 2;
    size |= size >> 4;
    size |= size >> 8;
    size |= size >> 16;
    size++;

    uint64_t base = ((base_addr >> base_shift) << base_shift) | type;
    uint64_t mask = (((-size & ((1ull<<max_phys_addr())-1)) >> base_shift) << base_shift) | valid_flag;

    prepare_set();

    write_msr(msr_mtrr_physbase(free_mtrr), base);
    write_msr(msr_mtrr_physmask(free_mtrr), mask);

    post_set();

    assert(read_msr(msr_mtrr_physbase(free_mtrr)) == base);
    assert(read_msr(msr_mtrr_physmask(free_mtrr)) == mask);


    return free_mtrr;
}

bool enabled()
{
    return available() && bit_check(read_msr(msr_def_type), 11);
}

VariableMTRR get_range(size_t id)
{
    assert(range_enabled(id));
    uint64_t mask = read_msr(msr_mtrr_physmask(id));
    uint64_t base = read_msr(msr_mtrr_physbase(id));

    return {base, mask};
}

bool range_enabled(size_t id)
{
    if (id < available_variable_ranges()) return false;
    uint64_t mask = read_msr(msr_mtrr_physmask(id));
    if (!(mask & valid_flag))
    {
        return false;
    }

    return true;
}

static uint32_t saved_cr4;
static uint64_t saved_mtrr_state;
void prepare_set()
{
    cli();

    uint32_t reg_cr0 = cr0();
    bit_set  (reg_cr0, 30);  // Cache-disable
    bit_clear(reg_cr0, 29);  // NW

    write_cr0(reg_cr0);
    wbinvd();

    uint32_t reg_cr4 = cr4();
    saved_cr4 = reg_cr4;
    if (bit_check(reg_cr4, 7)) // PGE bit
    {
        bit_clear(reg_cr4, 7);
        write_cr4(reg_cr4); // Flush TLBs
    }
    else
    {
        write_cr3(cr3()); // Flush TLBs
    }

    uint64_t mtrr_state = read_msr(msr_def_type);
    saved_mtrr_state = mtrr_state;
    bit_clear(mtrr_state, 11); // disable MTRRs
    write_msr(msr_def_type, mtrr_state);
}

void post_set()
{
    write_msr(msr_def_type, saved_mtrr_state); // restore state
    write_cr3(cr3()); // Flush TLBs

    if (!kgetenv("nocache"))
    {
        uint32_t reg_cr0 = cr0();
        bit_clear(reg_cr0, 30); // enable caches
        write_cr0(reg_cr0);
    }

    write_cr4(saved_cr4);

    sti();
}

}
