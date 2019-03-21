/*
defs.hpp

Copyright (c) 12 Yann BOUCHER (yann)

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
#ifndef SYSCALL_DEFS_HPP
#define SYSCALL_DEFS_HPP

#include "syscalls/syscall_list.hpp"
#include "syscall.h"

#define LINUX_SYSCALL_DEFAULT_IMPL(name, cnt, ret, args, ...) \
    ret name args \
    { \
        auto ret_val = DO_LINUX_SYSCALL(SYS_##name, cnt, ##__VA_ARGS__); \
 \
        if (ret_val < 0) \
        { \
            errno = -ret_val; \
            return (ret)-1; \
        } \
        else \
        { \
            return (ret)ret_val; \
        } \
    }

#define LUDOS_SYSCALL_DEFAULT_IMPL(name, cnt, ret, args, ...) \
    ret name args \
    { \
        auto ret_val = DO_LUDOS_SYSCALL(SYS_##name, cnt, ##__VA_ARGS__); \
 \
        if (ret_val < 0) \
        { \
            errno = -ret_val; \
            return (ret)-1; \
        } \
        else \
        { \
            return (ret)ret_val; \
        } \
    }

#endif // DEFS_HPP
