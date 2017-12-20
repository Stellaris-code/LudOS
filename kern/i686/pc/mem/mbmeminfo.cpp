/*
meminfo.cpp

Copyright (c) 31 Yann BOUCHER (yann)

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

#include "mbmeminfo.hpp"

#include "i686/pc/multiboot/multiboot.h"
#include "i686/mem/physallocator.hpp"

extern "C" int kernel_physical_end;

size_t MultibootMeminfo::free_frames()
{
    size_t counter = 0;

    for (multiboot_memory_map_t *mmap = mmap_addr;
         (uintptr_t)mmap < (uintptr_t)mmap_addr + mmap_length;
         mmap = (multiboot_memory_map_t*)((uintptr_t)mmap +
             mmap->size + sizeof(mmap->size))
         )
    {
        if (mmap->type == 1)
        {
            ++counter;
        }
    }


    return counter;
}

multiboot_memory_map_t *MultibootMeminfo::largest_frame()
{
    multiboot_memory_map_t* msf = mmap_addr;

    for (multiboot_memory_map_t *mmap = mmap_addr;
         (uintptr_t)mmap < (uintptr_t)mmap_addr + mmap_length;
         mmap = (multiboot_memory_map_t*)((uintptr_t)mmap +
             mmap->size + sizeof(mmap->size))
         )
    {
        if (mmap->size > msf->size)
        {
            msf = mmap;
        }
    }


    return msf;
}

multiboot_memory_map_t *MultibootMeminfo::frame(size_t idx)
{
    size_t counter = 0;

    for (multiboot_memory_map_t *mmap = mmap_addr;
         (uintptr_t)mmap < (uintptr_t)mmap_addr + mmap_length;
         mmap = (multiboot_memory_map_t*)((uintptr_t)mmap +
             mmap->size + sizeof(mmap->size))
         )
    {
        if (mmap->type == 1)
        {
            if (counter == idx)
            {
                return mmap;
            }
            ++counter;
        }
    }

    return nullptr;
}

size_t MultibootMeminfo::total_memory()
{
    size_t total = 0;

    for (size_t i { 0 }; i < free_frames(); ++i)
    {
        auto free_frame = frame(i);
        if (free_frame) total += free_frame->len;
    }

    // don't take kernel code memory in account
    total -= (uintptr_t)&kernel_physical_end;

    return total;
}

void MultibootMeminfo::init_alloc_bitmap()
{
    PhysPageAllocator::init();

    size_t frames = free_frames();
    for (size_t i { 0 }; i < frames; ++i)
    {
        multiboot_memory_map_t* mem_zone = frame(i);
        if (mem_zone)
        {
            PhysPageAllocator::mark_as_free(mem_zone->addr, mem_zone->len);
        }
    }
    // Mark kernel space as unavailable, protect multiboot info data
    PhysPageAllocator::mark_as_used(0, (reinterpret_cast<uintptr_t>(&kernel_physical_end + 0x160000)));
}
