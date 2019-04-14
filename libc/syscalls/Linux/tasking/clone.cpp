/*
clone.cpp

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

#include "syscalls/defs.hpp"

#include "errno.h"

#include <stdio.h>

extern "C"
{

int clone_fork(int flags, void* child_stack)
{
    auto ret_val = DO_LINUX_SYSCALL(SYS_clone, 2, flags, child_stack);

    if (ret_val < 0)
    {
        errno = -ret_val;
        return -1;
    }
    else
    {
        return ret_val;
    }
}

static int (*nonstack_fn_ptr) (void *__arg);
static void* nonstack_arg;

int clone (int (*fn) (void *__arg), void *child_stack,
           int flags, void *arg, ...)
{
    if ((uintptr_t)child_stack & 0xF  // not aligned
            || fn == nullptr || child_stack == nullptr
            )
    {
        errno = EINVAL;
        return -1;
    }

    // copy these to .data section variables since stack-referenced variables won't be accessible from the created thread
    nonstack_fn_ptr = fn;
    nonstack_arg = arg;

    auto ret_val = DO_LINUX_SYSCALL(SYS_clone, 2, flags, child_stack);

    if (ret_val < 0)
    {
        errno = -ret_val;
        return -1;
    }
    // parent, return thread's PID
    if (ret_val > 0)
        return ret_val;


    // created thread running here
    int ret = nonstack_fn_ptr(nonstack_arg);
    exit(ret);

    __builtin_unreachable();
    return 0;
}

}
