/*
abort.c

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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#if defined(__is_libk)
#include "panic.hpp"
#else
#include <signal.h>
#include "syscalls/syscall_list.hpp"
#endif

#include "utils/builtins.hpp"

#include "liballoc/liballoc.h"

__attribute__((__noreturn__))
void abort_impl(const char* file, size_t line, const char* fun)
{
#ifndef LUDOS_USER
    panic("Abort called at file '%s', line %d, function '%s'", file, line, fun);
#else
    // TODO: Abnormally terminate the process as if by SIGABRT.
    printf("abort()\n");
    kill(getpid(), SIGABRT);
#endif
    while (true) { }
    unreachable();
}

#undef abort

extern "C"
__attribute__((__noreturn__))
void abort(void)
{
#ifndef LUDOS_USER
    panic("Abort called");
#else
    // TODO: Abnormally terminate the process as if by SIGABRT.
    printf("abort()\n");
    kill(getpid(), SIGABRT);
#endif
    while (true) { }
    unreachable();
}

extern "C"
__attribute__((__noreturn__))
void _abort(void)
{
#ifndef LUDOS_USER
    panic("Abort called");
#else
    // TODO: Abnormally terminate the process as if by SIGABRT.
    printf("abort()\n");
    kill(getpid(), SIGABRT);
#endif
    while (true) { }
    unreachable();
}
