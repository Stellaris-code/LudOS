/*
memmap.hpp

Copyright (c) 09 Yann BOUCHER (yann)

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
#ifndef MEMMAP_HPP
#define MEMMAP_HPP

#include <stdint.h>
#include <string.h>
#include <limits.h>

class Memory
{
public:
    enum MmapFlags : uint32_t
    {
        Read         = 1<<0,
        Write        = 1<<1,
        User         = 1<<2,
        Uncached     = 1<<3,
        WriteThrough = 1<<4,
        Executable   = 1<<5,
        Sentinel     = 1<<6  // Used for custom page fault handlers, to signal an access to a page
    };

    enum CachingType
    {

    };

    static void* mmap(uintptr_t p_addr, size_t len, uint32_t flags = Read|Write);
    static void unmap(void* v_addr, size_t len);

    static void map_page(uintptr_t p_addr, void* v_addr, uint32_t flags = Memory::Read|Memory::Write);
    static void unmap_page(void* v_addr);
    static void remap_page(uintptr_t p_addr, void* v_addr, uint32_t flags);

    static bool is_mapped(const void* v_addr);

    static bool check_user_ptr(const void *v_addr, size_t size);

    static uintptr_t physical_address(const void* v_addr);

    static uintptr_t allocate_physical_page();
    static void release_physical_page(uintptr_t page);
    static size_t allocated_physical_pages();

    static uintptr_t allocate_virtual_page(size_t number, bool user);
    static void release_virtual_page(uintptr_t page);

    static constexpr size_t page_size()
    {
#ifdef ARCH_i686
        return 1 << 12;
#endif
    }

    static constexpr uintptr_t page(uintptr_t addr)
    {
        return (addr & ~(page_size()-1));
    }

    static constexpr uintptr_t offset(uintptr_t addr)
    {
        return addr & (page_size()-1);
    }

    static void phys_read(uintptr_t addr, void* buf, size_t size)
    {
        auto ptr = Memory::mmap(addr, size, Memory::Read);
        memcpy(buf, ptr, size);
        Memory::unmap(ptr, size);
    }

    static void phys_write(uintptr_t addr, const void* buf, size_t size)
    {
        auto ptr = Memory::mmap(addr, size, Memory::Write);
        memcpy(ptr, buf, size);
        Memory::unmap(ptr, size);
    }
};

#endif // MEMMAP_HPP
