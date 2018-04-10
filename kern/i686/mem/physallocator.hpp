/*
physallocator.hpp

Copyright (c) 11 Yann BOUCHER (yann)

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
#ifndef PHYSALLOCATOR_HPP
#define PHYSALLOCATOR_HPP

#include <stdint.h>

#include "utils/bitarray.hpp"

#include "paging.hpp"

class PhysPageAllocator
{
    friend class Meminfo;

public:
    static void init();

    static uintptr_t alloc_physical_page();
    static bool release_physical_page(uintptr_t p_addr);

    static void mark_as_used(uintptr_t addr, size_t size);
    static void mark_as_free(uintptr_t addr, size_t size);

    static void start_recording_allocs();
    static void stop_recording_allocs();

public:
    static int allocated_pages;
    static uintptr_t allocated_list[50000];

private:
    static uintptr_t find_free_page();
    static void clear_page(uintptr_t p_addr);

private:
    static inline bitarray<1024*1024, uint32_t> mem_bitmap; // 0 = free / 1 = used
    static inline size_t last_pos { 0 };
};

#endif // PHYSALLOCATOR_HPP
