/*
defs.hpp

Copyright (c) 10 Yann BOUCHER (yann)

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
#ifndef DEFS_HPP
#define DEFS_HPP

#ifdef __GNUC__
# define PRINTF_FMT(fmt_idx,arg1_idx, ...) \
    __attribute__((format (printf, fmt_idx, arg1_idx)))
#else
# define PRINTF_FMT(fmt_idx,arg1_idx)
#endif

#define KERNEL_VIRTUAL_BASE 0xC0000000
#define CURRENT_YEAR 2017
    
#define USES_ACPICA 0

#ifndef NDEBUG
#define LUDOS_DEBUG
#endif

#if 0
#define LUDOS_TESTING
#endif

#define QUOTE(str) #str
#define EXPAND_AND_QUOTE(str) QUOTE(str)

#define LUDOS_MINOR 1
#define LUDOS_MAJOR 0

#define LUDOS_VERSION_STRING "v" EXPAND_AND_QUOTE(LUDOS_MAJOR) "." EXPAND_AND_QUOTE(LUDOS_MINOR)

#include <stdint.h>

#endif // DEFS_HPP
