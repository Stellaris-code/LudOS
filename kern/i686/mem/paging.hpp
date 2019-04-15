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

#include "mem/memmap.hpp"

#include "i686/cpu/registers.hpp"

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
    uint8_t  global  : 1;
    uint8_t  os_claimed : 1;
    uint8_t  data    : 2;
    uint32_t pt_addr : 20;
};
static_assert(sizeof(PDEntry) == 4);

using PageDirectory = kpp::array<PDEntry, 1024>;

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
    uint8_t  os_claimed : 1;
    uint8_t  data    : 2;
    uint32_t phys_addr: 20;
};
static_assert(sizeof(PTEntry) == 4);

using PageTable = kpp::array<PTEntry, 1024>;

struct PagingInformation
{
    alignas(4096) PageDirectory                 page_directory;
    alignas(4096) kpp::array<PageTable, 1024>   page_tables;
};

class Paging
{
public:
    enum ReleaseFlags
    {
        FreePage,
        KeepClaimed
    };

public:
    static void init();

    static uintptr_t alloc_virtual_page(size_t number = 1, bool user = false);
    static bool release_virtual_page(uintptr_t v_addr, size_t number = 1, ReleaseFlags flags = FreePage);

    static void map_page(uintptr_t p_addr, void* v_addr, uint32_t flags = Memory::Read|Memory::Write);
    static void unmap_page(void* v_addr);

    static void identity_map(uintptr_t p_addr, size_t size, uint32_t flags = Memory::Read|Memory::Write);

    static uintptr_t physical_address(const void *v_addr);

    static bool is_mapped(const void* v_addr);

    static bool check_user_ptr(const void* v_addr, size_t size);

    static void unmap_user_space();

    static void create_paging_info(PagingInformation& info);

public:
    static constexpr uint32_t page_size { 1 << 12 };
    static constexpr uint32_t ram_maxpage { 1024*1023 };

private:
    static bool page_fault_handler(registers *regs);

private:
    static void map_kernel(PagingInformation& info);
    static PTEntry *page_entry(uintptr_t addr);
};

#endif // PAGING_HPP
