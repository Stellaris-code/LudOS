/*
memutils.hpp

Copyright (c) 19 Yann BOUCHER (yann)

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
#ifndef MEMUTILS_HPP
#define MEMUTILS_HPP

#include <stdint.h>
#include <math.h>
#include <stdio.h>

#include "utils/logging.hpp"

inline int meminfo_intlog(double base, double x)
{
    return (int)(log10(x) / log10(base));
}

inline void dump(const void* address, size_t amnt)
{
    const size_t line_size = 80;
    const size_t byte_disp_size = 3; // 2 + 1 space
    const size_t bytes_per_line = line_size/(byte_disp_size) - meminfo_intlog(16, amnt) + 1;

    const size_t cols = amnt/bytes_per_line;
    for (size_t i { 0 }; i < cols; ++i)
    {
        kprintf("%x ", i*bytes_per_line);
        for (size_t j { 0 }; j < meminfo_intlog(16, amnt) - meminfo_intlog(16, i*bytes_per_line); ++j)
        {
            kprintf(" ");
        }

        for (size_t j { 0 }; j < bytes_per_line; ++j)
        {
            kprintf("%02x ", reinterpret_cast<const uint8_t*>(address)[i*bytes_per_line + j]);
        }
        kprintf("\n");
    }
}

inline void dump_serial(const void* address, size_t amnt)
{
    const size_t line_size = 80;
    const size_t byte_disp_size = 3; // 2 + 1 space
    const size_t bytes_per_line = line_size/(byte_disp_size) - meminfo_intlog(16, amnt) + 1;

    const size_t cols = amnt/bytes_per_line;
    for (size_t i { 0 }; i < cols; ++i)
    {
        log_serial("%x ", i*bytes_per_line);
        for (size_t j { 0 }; j < meminfo_intlog(16, amnt) - meminfo_intlog(16, i*bytes_per_line); ++j)
        {
            log_serial(" ");
        }

        for (size_t j { 0 }; j < bytes_per_line; ++j)
        {
            log_serial("%02x ", reinterpret_cast<const uint8_t*>(address)[i*bytes_per_line + j]);
        }
        log_serial("\n");
    }
}

#endif // MEMUTILS_HPP
