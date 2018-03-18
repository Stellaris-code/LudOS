/*
mmap.cpp

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

#include "mem/memmap.hpp"

#include "utils/logging.hpp"

#include "paging.hpp"

void *Memory::mmap(void *p_addr, size_t len, uint32_t flags)
{
    size_t offset = (uintptr_t)p_addr & 0xFFF;

    len += offset;

    size_t page_num = len/Paging::page_size + (len%Paging::page_size?1:0);

    assert(len);

    uintptr_t pages = Paging::alloc_virtual_page(page_num);
    for (size_t i { 0 }; i < page_num; ++i)
    {
        Paging::map_page(reinterpret_cast<uint8_t*>(p_addr) + i*Paging::page_size,
                         reinterpret_cast<void*>(pages + i*Paging::page_size),
                         flags);
    }

    return reinterpret_cast<void*>(pages + offset);
}

void Memory::unmap(void *v_addr, size_t len)
{
    size_t page_num = len/Paging::page_size + (len%Paging::page_size?1:0);

    for (size_t i { 0 }; i < page_num; ++i)
    {
        Paging::unmap_page(reinterpret_cast<uint8_t*>(v_addr) + i*Paging::page_size);
    }
}

bool Memory::is_mapped(void *v_addr)
{
    return Paging::is_mapped(v_addr);
}

uintptr_t Memory::physical_address(const void *v_addr)
{
    return Paging::physical_address(v_addr);
}

size_t Memory::page_size()
{
    return Paging::page_size;
}
