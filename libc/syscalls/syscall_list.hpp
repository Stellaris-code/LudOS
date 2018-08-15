/*
syscalls.hpp

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
#ifndef LIBC_SYSCALLS_HPP
#define LIBC_SYSCALLS_HPP

#include <stdint.h>

#ifndef LUDOS_USER
//#error Reserved for userspace
#endif

#define LUDOS_SYSCALL_DEF_KERNEL(...)
#define LUDOS_SYSCALL_DEF_USER(num, name, ret, ...) \
    ret name(__VA_ARGS__);

#define LINUX_SYSCALL_DEF_KERNEL(...)
#define LINUX_SYSCALL_DEF_USER(num, name, ret, ...) \
    ret name(__VA_ARGS__);

#define USER_PTR(type) type*

#include "syscalls/syscall_list.def"

#undef LUDOS_SYSCALL_DEF_USER
#undef LINUX_SYSCALL_DEF_USER
#undef LUDOS_SYSCALL_DEF_KERNEL
#undef LINUX_SYSCALL_DEF_KERNEL
#undef USER_PTR

#include "syscalls/syscalls.hpp"

#define DETAIL_LUDOS_ID 0
#define DETAIL_LINUX_ID 1


#define syscall(num, ...) common_syscall(1, num, __VA_ARGS__)
#define ludos_syscall(num, ...) common_syscall(0, num, __VA_ARGS__)


pid_t wait(int *status);

#endif // SYSCALLS_HPP
