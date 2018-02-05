/*
meminfo.hpp

Copyright (c) 23 Yann BOUCHER (yann)

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
#ifndef MEMINFO_HPP
#define MEMINFO_HPP

#include <stdint.h>

extern "C" unsigned long long l_allocated;		///< Running total of allocated memory.
extern "C" unsigned long long l_inuse;		///< Running total of used memory.
extern "C" unsigned long long l_max_inuse;

struct MemoryInfo
{
    static inline size_t available_bytes { 0 };

    static inline size_t total()
    {
        return MemoryInfo::available_bytes;
    }
    static inline size_t free()
    {
        return MemoryInfo::available_bytes - l_inuse;
    }
    static inline size_t allocated()
    {
        return l_allocated;
    }
    static inline size_t used()
    {
        return l_inuse;
    }
    static inline size_t max_usage()
    {
        return l_max_inuse;
    }

    static inline size_t usage_ratio()
    {
        return used()*100/total();
    }
};

#endif // MEMINFO_HPP
