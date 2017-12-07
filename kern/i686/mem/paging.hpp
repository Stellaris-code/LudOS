/*
paging.hpp

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
#ifndef PAGING_HPP
#define PAGING_HPP

#include <stdint.h>

#include "utils/bitarray.hpp"

#include <array.hpp>

struct multiboot_mmap_entry;
typedef struct multiboot_mmap_entry multiboot_memory_map_t;

struct [[gnu::packed]] PDEntry
{
    uint8_t  present : 1;
    uint8_t  write   : 1;
    uint8_t  user    : 1;
    uint8_t  wt      : 1;
    uint8_t  cd      : 1;
    uint8_t  accessed: 1;
    uint8_t  zero    : 1;
    uint8_t  size    : 1;
    uint8_t  ignored : 1;
    uint8_t  data    : 3;
    uint32_t pt_addr : 20;
};
static_assert(sizeof(PDEntry) == 4);

using PageDirectory = std::array<PDEntry, 1024>;

struct [[gnu::packed]] PTEntry
{
    uint8_t  present  : 1;
    uint8_t  write    : 1;
    uint8_t  user     : 1;
    uint8_t  wt       : 1;
    uint8_t  cd       : 1;
    uint8_t  accessed : 1;
    uint8_t  dirty    : 1;
    uint8_t  zero     : 1;
    uint8_t  global   : 1;
    uint8_t  data     : 3;
    uint32_t phys_addr: 20;
};
static_assert(sizeof(PTEntry) == 4);

using PageTable = std::array<PTEntry, 1024>;

class Paging
{
    friend class Meminfo;

public:
    static void init();

    static uintptr_t alloc_page_frame(size_t number = 1);

    static bool release_page_frame(uintptr_t p_addr, size_t number = 1);

    static void mark_as_used(uintptr_t addr, size_t size);

public:
    static constexpr uint32_t ram_maxpage { ((~0u >> 12u) + 1) };
    static constexpr uint32_t page_size { 1 << 12 };

private:
    static constexpr uintptr_t page(uintptr_t ptr)
    {
        return ptr >> 12;
    }

    static void init_page_directory();
    static void identity_map();
    static void map_page(void* phys, void *virt, uint32_t flags);

private:
    static inline bitarray<ram_maxpage, uint32_t> mem_bitmap; // 0 = free / 1 = used
};

#endif // PAGING_HPP
