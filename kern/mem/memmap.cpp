/*
memmap.cpp

Copyright (c) 21 Yann BOUCHER (yann)

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

#include "memmap.hpp"

#include <assert.h>

void *Memory::vmalloc(size_t pages, uint32_t flags)
{
    uint8_t* addr = reinterpret_cast<uint8_t*>(Memory::allocate_virtual_page(pages, false));
    for (size_t i { 0 }; i < pages; ++i)
    {
        void* virtual_page  = (uint8_t*)addr + i*Memory::page_size();
        uintptr_t physical_page = Memory::allocate_physical_page();
        Memory::map_page(physical_page, virtual_page, flags);
    }

    return addr;
}

void Memory::vfree(void *base, size_t pages)
{
    assert((uintptr_t)base % Memory::page_size() == 0);

    for (size_t i { 0 }; i < pages; ++i)
    {
        void* virtual_page = (uint8_t*)base + i*Memory::page_size();

        uintptr_t physical_page = Memory::physical_address(virtual_page);

        Memory::release_physical_page(physical_page);
        Memory::unmap_page(virtual_page);
    }
}
