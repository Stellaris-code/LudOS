/*
do_syscall.h

Copyright (c) 15 Yann BOUCHER (yann)

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
#ifndef DO_SYSCALL_H
#define DO_SYSCALL_H

#include "pp_utils.h"

#define ASMFMT_0()
#define ASMFMT_1(arg1) \
    , "b" (arg1)
#define ASMFMT_2(arg1, arg2) \
    , "b" (arg1), "c" (arg2)
#define ASMFMT_3(arg1, arg2, arg3) \
    , "b" (arg1), "c" (arg2), "d" (arg3)
#define ASMFMT_4(arg1, arg2, arg3, arg4) \
    , "b" (arg1), "c" (arg2), "d" (arg3), "S" (arg4)
#define ASMFMT_5(arg1, arg2, arg3, arg4, arg5) \
    , "b" (arg1), "c" (arg2), "d" (arg3), "S" (arg4), "D" (arg5)
#define ASMFMT_6(arg1, arg2, arg3, arg4, arg5, arg6) \
, "b" (arg1), "c" (arg2), "d" (arg3), "S" (arg4), "D" (arg5), "bp" (arg6)

#define DO_LINUX_SYSCALL(sys_no, cnt, ...) \
        DO_SYSCALL_IMPL(0x80, sys_no, cnt, __VA_ARGS__)
#define DO_LUDOS_SYSCALL(sys_no, cnt, ...) \
        DO_SYSCALL_IMPL(0x70, sys_no, cnt, __VA_ARGS__)

#define DO_SYSCALL_IMPL(int_no, sys_no, cnt, ...) \
    ({ \
    int ret_val; \
    asm volatile \
    ("mov %1, %%eax\n" \
     "int $" #int_no "\n" \
    :"=a"(ret_val) \
    :"i"(sys_no)\
    ASMFMT_##cnt(__VA_ARGS__)\
    :"memory"\
    );\
    ret_val; \
    })
#endif // DO_SYSCALL_H
