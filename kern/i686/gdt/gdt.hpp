/*
gdt.hpp

Copyright (c) 26 Yann BOUCHER (yann)

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
#ifndef GDT_HPP
#define GDT_HPP

#include <stdint.h>

extern "C"
void gdt_flush(uint32_t addr);

namespace gdt
{

struct [[gnu::packed]] entry
{
   uint16_t limit_low;           // The lower 16 bits of the limit.
   uint16_t base_low;            // The lower 16 bits of the base.
   uint8_t  base_middle;         // The next 8 bits of the base.
   uint8_t  access;              // Access flags, determine what ring this segment can be used in.
   uint8_t  granularity;
   uint8_t  base_high;           // The last 8 bits of the base.
};

struct [[gnu::packed]] ptr
{
   uint16_t limit;               // The upper 16 bits of all selector limits.
   uint32_t base;                // The address of the first gdt_entry_t struct.
};

void set_gate(size_t num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran);

void init();

void flush();

extern entry entries[9];
extern ptr gdt_ptr;

static constexpr uint16_t null_selector { 0 };
static constexpr uint16_t kernel_code_selector { 1 };
static constexpr uint16_t kernel_data_selector { 2 };
static constexpr uint16_t user_code_selector { 3 };
static constexpr uint16_t user_data_selector { 4 };
static constexpr uint16_t tss_selector { 5 };

}

#endif // GDT_HPP
