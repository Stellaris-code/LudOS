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

#include "halt.hpp"
#include "panic.hpp"
#include "utils/logging.hpp"
#include "utils/bitops.hpp"

#include "i686/interrupts/isr.hpp"

#include "physallocator.hpp"

extern "C" int kernel_physical_end;

alignas(4096) static PageDirectory                 page_directory;
alignas(4096) static std::array<PageTable, 1024>   page_tables;

void Paging::init()
{
    cli();
    init_page_directory();

    isr::register_handler(isr::PageFault, page_fault_handler);

    uint32_t pd_addr { reinterpret_cast<uint32_t>(page_directory.data()) - KERNEL_VIRTUAL_BASE };
    uint32_t cr4_var = cr4();
    bit_clear(cr4_var, 4); // disable 4MB pages

    asm volatile ("mov %0, %%cr3\n"
                  "mov %1, %%cr4\n"
                  "\n"::"r"(pd_addr), "r"(cr4_var));

    m_initialized = true;

    sti();
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
    size_t page_num = size/page_size + (size%page_size?1:0);

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

// TODO : last_pos
uintptr_t Paging::alloc_virtual_page(size_t number)
{
    assert(number);

    PTEntry* entries = page_entry(0);

    uintptr_t addr { 0 };

    size_t counter { 0 };

    number += 2;

    // keep first 1M free
    for (size_t i { 0x100 }; i < ram_maxpage; ++i)
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
            for (size_t j { 1 }; j < number-1; ++j)
            {
                entries[addr + j].present = true;
            }
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
        entry[i].present = false;
        asm volatile ("invlpg (%0)"::"r"(reinterpret_cast<uint8_t*>(v_addr) + i*page_size) : "memory");
    }

    return true;
}

PageDirectory *Paging::get_page_directory()
{
    if (!m_initialized)
    {
        return &page_directory;
    }
    else
    {
        return reinterpret_cast<PageDirectory*>(0xFFFFF000);
    }
}

void Paging::init_page_directory()
{
    memset(page_directory.data(), 0, page_directory.size()*sizeof(PDEntry));
    for (size_t i { 0 }; i < page_tables.size(); ++i)
    {
        memset(&page_tables[i], 0, page_tables[i].size()*sizeof(PTEntry));
        page_directory[i].pt_addr = (reinterpret_cast<uint32_t>(&page_tables[i]) - KERNEL_VIRTUAL_BASE) >> 12;
        page_directory[i].present = true;
        page_directory[i].write = true;
    }

    map_kernel();

    // map last dir entry to itself
    page_directory.back().pt_addr = (reinterpret_cast<uint32_t>(page_directory.data()) - KERNEL_VIRTUAL_BASE) >> 12;
    page_directory.back().write = true;
    page_directory.back().present = true;
}

void Paging::map_kernel()
{
    PTEntry* entry = page_entry(KERNEL_VIRTUAL_BASE);

    for (uint32_t addr { 0 }; addr <= reinterpret_cast<uint32_t>(&kernel_physical_end) + page_size; addr+=page_size, ++entry)
    {
        entry->phys_addr = addr >> 12;
        entry->present = true;
        entry->write = true;
        entry->user = false;
    }
}
