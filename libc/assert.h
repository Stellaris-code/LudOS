/*
assert.h

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

#include <stdint.h>

#include "utils/defs.hpp"

inline void impl_assert(bool cond, const char* strcond, const char* file, size_t line, const char* fun);
PRINTF_FMT(6, 7)
inline void impl_assert_msg(bool cond, const char* strcond, const char* file, size_t line, const char* fun, const char* fmt, ...);

#ifndef assert
#define assert(cond) ::impl_assert(cond, #cond, __FILE__, __LINE__, __FUNCTION__)
#endif

#ifndef assert_mesg
#define assert_msg(cond, fmt, ...) ::impl_assert_msg(cond, #cond, __FILE__, __LINE__, __FUNCTION__, fmt, ##__VA_ARGS__);
#endif

#ifndef __ASSERT_H
#define __ASSERT_H

#include <stdint.h>
#include <stdarg.h>

#include "../kern/utils/logging.hpp"
#include "halt.hpp"
#include "panic.hpp"

PRINTF_FMT(1, 2)
void err(const char * __restrict fmt, ...);

#ifndef NDEBUG
#define error_impl panic
#elif
#define error_impl err
#endif
//"Reason : '" msg "'\n"

inline void impl_assert(bool cond, const char* strcond, const char* file, size_t line, const char* fun)
{
    if (!cond)
    {
        error_impl("Assert in file '%s', '%s', line %zd : cond '%s' is false\n", file, fun, line, strcond);
    }
}
inline void impl_assert_msg(bool cond, const char* strcond, const char* file, size_t line, const char* fun, const char* fmt, ...)
{
    if (!cond)
    {
        char msg[512];

        va_list va;
        va_start(va, fmt);
        kvsnprintf(msg, sizeof(msg), fmt, va);
        va_end(va);

        error_impl("Assert in file '%s', '%s', line %zd : cond '%s' is false\nReason : '%s'\n", file, fun, line, strcond, msg);
    }
}

#endif // __ASSERT_H
