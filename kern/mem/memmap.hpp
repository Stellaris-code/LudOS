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
        NoExec       = 1<<5
    };

    static void* mmap(void* p_addr, size_t len, uint32_t flags = Read|Write);
    static void unmap(void* v_addr, size_t len);

    static uintptr_t physical_address(const void* v_addr);
};

#endif // MEMMAP_HPP
