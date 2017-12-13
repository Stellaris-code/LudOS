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

#include "meminfo.hpp"

#include "../multiboot/multiboot.h"
#include "utils/addr.hpp"
#include "i686/mem/physallocator.hpp"

extern "C" int kernel_physical_end;

size_t Meminfo::free_frames()
{
    size_t counter = 0;

    for (multiboot_memory_map_t *mmap = Meminfo::mmap_addr;
         reinterpret_cast<uintptr_t>(mmap) < phys(info->mmap_addr) + info->mmap_length;
         mmap = reinterpret_cast<multiboot_memory_map_t*>(
             reinterpret_cast<uintptr_t>(mmap)
             + mmap->size + sizeof(mmap->size))
         )
    {
        if (mmap->type == 1)
        {
            ++counter;
        }
    }


    return counter;
}

multiboot_memory_map_t *Meminfo::largest_frame()
{
    multiboot_memory_map_t* msf = Meminfo::mmap_addr;

    for (multiboot_memory_map_t *mmap = Meminfo::mmap_addr;
         reinterpret_cast<uintptr_t>(mmap) < phys(info->mmap_addr) + info->mmap_length;
         mmap = reinterpret_cast<multiboot_memory_map_t*>(
             reinterpret_cast<uintptr_t>(mmap)
             + mmap->size + sizeof(mmap->size))
         )
    {
        if (mmap->size > msf->size)
        {
            msf = mmap;
        }
    }


    return msf;
}

multiboot_memory_map_t *Meminfo::frame(size_t idx)
{
    size_t counter = 0;

    for (multiboot_memory_map_t *mmap = Meminfo::mmap_addr;
         reinterpret_cast<uintptr_t>(mmap) < phys(info->mmap_addr) + info->mmap_length;
         mmap = reinterpret_cast<multiboot_memory_map_t*>(
             reinterpret_cast<uintptr_t>(mmap)
             + mmap->size + sizeof(mmap->size))
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

size_t Meminfo::total_memory()
{
    size_t total = 0;

    for (size_t i { 0 }; i < free_frames(); ++i)
    {
        auto free_frame = frame(i);
        if (free_frame) total += free_frame->len;
    }

    return total;
}

void Meminfo::init_alloc_bitmap()
{
    // TODO : adapt for 64-bit
    PhysPageAllocator::mark_as_used(0, 0xFFFFFFFF);

    size_t free_frames = Meminfo::free_frames();
    for (size_t i { 0 }; i < free_frames; ++i)
    {
        multiboot_memory_map_t* mem_zone = Meminfo::frame(i);
        if (mem_zone)
        {
            PhysPageAllocator::mark_as_free(mem_zone->addr, mem_zone->len);
        }
    }
    // Mark kernel space as unavailable, protect multiboot info data
    PhysPageAllocator::mark_as_used(0, (reinterpret_cast<uintptr_t>(&kernel_physical_end + 0x80000)));
}
