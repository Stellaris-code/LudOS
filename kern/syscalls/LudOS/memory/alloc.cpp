/*
alloc.cpp

Copyright (c) 18 Yann BOUCHER (yann)

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

#include <errno.h>

#include "i686/mem/physallocator.hpp"

#include "tasking/process.hpp"

int sys_alloc_pages(int pages)
{
    return Process::current().allocate_pages(pages);
}

int sys_free_pages(uintptr_t ptr, int pages)
{
    //    assert(ptr % Paging::page_size == 0);

    //    for (size_t i { 0 }; i < pages; ++i)
    //    {
    //        void* virtual_page  = (uint8_t*)ptr + i*Paging::page_size;
    //        void* physical_page = (void*)Memory::physical_address(virtual_page);
    //        PhysPageAllocator::release_physical_page((uintptr_t)physical_page);
    //        Paging::unmap_page((uint8_t*)ptr + i*Paging::page_size);

    //        auto& p_pages = Process::current().allocated_pages;

    //        assert(p_pages.count((uintptr_t)virtual_page));
    //        p_pages.erase((uintptr_t)virtual_page);
    //    }

    //    return EOK;
    return Process::current().release_pages(ptr, pages) ? EOK : -ENOMEM;
}
