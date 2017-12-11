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

#include "i686/interrupts/isr.hpp"

#define LASTPOS_OPTIMIZATION 1

extern "C" int kernel_physical_end;

alignas(4096) static PageDirectory                 page_directory;
alignas(4096) static std::array<PageTable, 1024>   page_tables;

void Paging::init()
{
    cli();
    init_page_directory();

    isr::register_handler(isr::PageFault, page_fault_handler);

    uint32_t pd_addr { reinterpret_cast<uint32_t>(page_directory.data()) };
    uint32_t cr = cr0() | 0x80010000;

    asm volatile ("mov %0, %%cr3\n"
                  "mov %1, %%cr0\n"::"r"(pd_addr), "r"(cr));
    sti();
}

#if LASTPOS_OPTIMIZATION
static size_t last_pos { 0 };
#endif

uintptr_t Paging::alloc_physical_page(size_t number)
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

bool Paging::release_physical_page(uintptr_t p_addr, size_t number)
{
    size_t base_page = page(virt(p_addr));
    bool released = false;

    for (size_t i { 0 }; i < number; ++i)
    {
        released |= mem_bitmap[base_page+i];
        mem_bitmap[base_page+i] = false;
    }

#if LASTPOS_OPTIMIZATION
    if (number > 2 && released) last_pos = base_page;
#endif

    return released;
}

void Paging::map_page(void *p_addr, void *v_addr, uint32_t flags)
{
    auto entry = page_entry(reinterpret_cast<uintptr_t>(v_addr));

    //log_serial("Mapping : %p->%p\n", p_addr, v_addr);

    entry->phys_addr = reinterpret_cast<uintptr_t>(p_addr) >> 12;

    entry->write = !!(flags & Memory::Write);
    entry->cd = !!(flags & Memory::Uncached);
    entry->wt = !!(flags & Memory::WriteThrough);
    entry->user = !!(flags & Memory::User);

    entry->present = true;
}

void Paging::unmap_page(void *v_addr)
{
    release_virtual_page(reinterpret_cast<uintptr_t>(v_addr));
}

void Paging::identity_map(void *p_addr, size_t size, uint32_t flags)
{
    size_t page_num = size/page_size + (size%page_size?0:1);

    for (size_t i { 0 }; i < page_num; ++i)
    {
        map_page((uint8_t*)p_addr + i * page_size, (uint8_t*)p_addr + i * page_size, flags);
    }
}

uintptr_t Paging::physical_address(const void *v_addr)
{
    size_t offset = (uintptr_t)v_addr & 0xFFF;

    auto entry = page_entry(reinterpret_cast<uintptr_t>(v_addr));

    if (!entry->present) return 0;

    return (entry->phys_addr << 12) + offset;
}

uintptr_t Paging::alloc_virtual_page(size_t number)
{
    assert(number);

    number += 2; // put a barrier around

    PTEntry* entries = page_entry(0);

    uintptr_t addr { 0 };

    size_t counter { 0 };

    for (size_t i { 0 }; i < 1024*1024; ++i)
    {
        if (!entries[i].present)
        {
            if (counter++ == 0) addr = i;
        }
        else
        {
            counter = 0;
        }

        if (counter == number)
        {
            return addr * page_size + page_size;
        }
    }
    panic("no more virtual addresses available");
    return 0;
}

bool Paging::release_virtual_page(uintptr_t v_addr, size_t number)
{
    auto entry = page_entry(v_addr);

    for (size_t i { 0 }; i < number; ++i)
    {
        release_physical_page(entry[i].phys_addr << 12);
        entry[i].present = false;
        asm volatile ("invlpg (%0)"::"r"(reinterpret_cast<uint8_t*>(v_addr) + i*page_size) : "memory");
    }

    return true;
}

void Paging::mark_as_used(uintptr_t addr, size_t size)
{
    for (size_t i { page(addr) }; i < page(addr + size); ++i)
    {
        mem_bitmap[i] = true;
    }
}

PageDirectory *Paging::get_page_directory()
{
    return reinterpret_cast<PageDirectory*>(0xFFFFF000);
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

    // map last dir entry to itself
    page_directory.back().pt_addr = reinterpret_cast<uint32_t>(page_directory.data()) >> 12;
    page_directory.back().write = true;
    page_directory.back().present = true;
}

void Paging::identity_map()
{
    PTEntry* entry = &page_tables[0][0];

    for (uint32_t addr { 0 }; addr <= reinterpret_cast<uint32_t>(&kernel_physical_end) + page_size; addr+=0x1000, ++entry)
    {
        entry->phys_addr = addr >> 12;
        entry->present = true;
        entry->write = true;
        entry->user = false;
    }
}

bool Paging::page_fault_handler(const registers *regs)
{
    panic_regs = regs;
    panic("Page fault !\n");
    return false;
}
