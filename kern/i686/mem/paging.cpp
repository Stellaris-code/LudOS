/*
paging.cpp

Copyright (c) 30 Yann BOUCHER (yann)

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

#include "paging.hpp"

#include <stdint.h>
#include "utils/addr.hpp"

#include "halt.hpp"
#include "panic.hpp"
#include "utils/logging.hpp"

#include "i686/cpu/registers.hpp"

#define LASTPOS_OPTIMIZATION 1

extern "C" int kernel_physical_end;

alignas(4096) static PageDirectory                 page_directory;
alignas(4096) static std::array<PageTable, 1024>   page_tables;

void Paging::init()
{
    cli();
    init_page_directory();

    uint32_t pd_addr { reinterpret_cast<uint32_t>(page_directory.data()) };
    uint32_t cr = cr0() | 0x80010000;

    uint32_t var = *((uint32_t*)page_directory.data());

    asm volatile ("mov %0, %%cr3\n"
                  "mov %1, %%cr0\n"::"r"(pd_addr), "r"(cr));
    sti();
}

#if LASTPOS_OPTIMIZATION
static size_t last_pos { 0 };
#endif

uintptr_t Paging::alloc_page_frame(size_t number)
{
#if LASTPOS_OPTIMIZATION
loop:
#endif

    size_t counter { 0 };
    size_t page_addr { 0 };

#if LASTPOS_OPTIMIZATION
    size_t i { last_pos };
#else
    size_t i { 0 };
#endif
    for (; i < mem_bitmap.array_size; ++i)
    {
        if (!mem_bitmap[i])
        {
            ++counter;
            if (counter == 1)
            {
                page_addr = i;
            }
        }
        else
        {
            counter = 0;
        }

        if (counter >= number)
        {
            for (size_t j { page_addr }; j < page_addr+counter; ++j)
            {
                assert(!mem_bitmap[j]);
                mem_bitmap[j] = true;
            }
#if LASTPOS_OPTIMIZATION
            last_pos = page_addr+counter;
#endif
            return phys(page_addr*page_size);
        }
    }

#if LASTPOS_OPTIMIZATION
    if (last_pos != 0)
    {
        last_pos = 0;
        goto loop;
    }
#endif

    log_serial("Out of memory\n");
    panic("Out of memory !\n");

    return 0;
}

bool Paging::release_page_frame(uintptr_t p_addr, size_t number)
{
    size_t base_page = page(virt(p_addr));
    bool released = false;

    for (size_t i { 0 }; i < number; ++i)
    {
        released |= mem_bitmap[base_page+i];
        mem_bitmap[base_page+i] = false;
    }

#if LASTPOS_OPTIMIZATION
    if (number > 2) last_pos = base_page;
#endif

    return released;
}

void Paging::mark_as_used(uintptr_t addr, size_t size)
{
    for (size_t i { page(addr) }; i < page(addr + size); ++i)
    {
        mem_bitmap[i] = true;
    }
}

void Paging::init_page_directory()
{
    memset(page_directory.data(), 0, sizeof(page_directory));
    for (size_t i { 0 }; i < page_tables.size(); ++i)
    {
        memset(&page_tables[i], 0, sizeof(page_tables[i]));
        page_directory[i].pt_addr = reinterpret_cast<uint32_t>(&page_tables[i]) >> 12;
        page_directory[i].present = true;
        page_directory[i].write = true;
    }

    identity_map();

    // map last table to itself
    page_tables[1023][1023].phys_addr = reinterpret_cast<uint32_t>(page_directory.data()) >> 12;
    page_tables[1023][1023].write = true;
    page_tables[1023][1023].present = true;
}

void Paging::identity_map()
{
    PTEntry* entry = &page_tables[0][0];

    for (uint32_t addr { 0 }; addr <= reinterpret_cast<uint32_t>(&kernel_physical_end) + 0x1000; addr+=0x1000, ++entry)
    {
        entry->phys_addr = addr >> 12;
        entry->present = true;
        entry->write = true;
        entry->user = false;
    }
}

void Paging::map_page(void *phys, void *virt, uint32_t flags)
{
    // Make sure that both addresses are page-aligned.

    uint32_t pdindex = reinterpret_cast<uint32_t>(virt) >> 22;
    uint32_t ptindex = reinterpret_cast<uint32_t>(virt) >> 12 & 0x03FF;

    PageDirectory   * pd = reinterpret_cast<PageDirectory*>(0xFFFFF000);

    PageTable       * pt = reinterpret_cast<PageTable*>((*pd)[pdindex].pt_addr << 12);

    (*pt)[ptindex].phys_addr = reinterpret_cast<uint32_t>(phys) >> 12;
    (*pt)[ptindex].present = true;
}
