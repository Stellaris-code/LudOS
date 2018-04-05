/*
syscall.hpp

Copyright (c) 11 Yann BOUCHER (yann)

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
#ifndef SYSCALL_HPP
#define SYSCALL_HPP

#include <stdint.h>

#include "utils/user_ptr.hpp"

constexpr uint8_t linux_syscall_int = 0x80;
constexpr uint8_t ludos_syscall_int = 0x70;

extern volatile bool processing_syscall;

void init_syscalls();

#define LUDOS_SYSCALL_DEF(num, name, ret, ...) \
    ret sys_##name(__VA_ARGS__); \
    constexpr size_t SYS_##name = num;

#define LINUX_SYSCALL_DEF(num, name, ret, ...) \
    ret sys_##name(__VA_ARGS__); \
    constexpr size_t SYS_##name = num;

#define USER_PTR(type) user_ptr<type>

#include "syscall_list.def"

#undef LUDOS_SYSCALL_DEF
#undef LINUX_SYSCALL_DEF
#undef USER_PTR

#endif // SYSCALL_HPP
