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

#include <string.hpp>

#include "utils/logging.hpp"

inline size_t meminfo_intlog(double base, double x)
{
    return (size_t)(log10(x) / log10(base));
}

inline size_t meminfo_ipow(size_t base, size_t exp)
{
    size_t result = 1;
    while (exp)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }

    return result;
}

inline std::string human_readable_size(size_t bytes)
{
    const size_t unit = 1024;
    if (bytes < unit) return std::to_string(bytes) + " B";
    const size_t exp = meminfo_intlog(unit, bytes);
    const std::string units = "KMGTPE";
    const std::string suffix = units[exp-1] + std::string("iB");

    char buf[16];
    ksnprintf(buf, 16, "%.1f ", (double)bytes / meminfo_ipow(unit, exp));

    return buf + suffix;
};

inline void dump(const void* address, size_t amnt)
{
    const size_t line_size = 80;
    const size_t byte_disp_size = 3; // 2 + 1 space
    const size_t bytes_per_line = line_size/(byte_disp_size) - meminfo_intlog(16, amnt) + 1;

    const size_t cols = amnt/bytes_per_line; // TODO : fix this
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
