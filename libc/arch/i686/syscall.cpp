/*
syscall.cpp

Copyright (c) 14 Yann BOUCHER (yann)

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

#define LIBK_SYSCALL_IMPL
#include "syscall.h"

#include "errno.h"

#include "i686/syscalls/syscall.hpp"
#include "syscalls/LudOS/syscalls.hpp"

extern "C" int do_ludos_syscall(uint32_t no, uint32_t args, uint32_t* arg_table);
extern "C" int do_linux_syscall(uint32_t no, uint32_t args, uint32_t* arg_table);

static SyscallEntry libc_ludos_syscall_table[max_syscalls];
static SyscallEntry libc_linux_syscall_table[max_syscalls];

static bool is_ready = false;

int common_syscall(size_t type, size_t no, ...)
{
    if (!is_ready)
    {
        uint32_t arg_table[] = {(uint32_t)libc_ludos_syscall_table, (uint32_t)libc_linux_syscall_table};
        do_ludos_syscall(0, 2, arg_table); // fetch the syscall tables

        is_ready = true;
    }

    auto& table = (type == 0) ? libc_ludos_syscall_table : libc_linux_syscall_table;

    if (no > max_syscalls || table[no].arg_cnt == invalid_syscall_magic)
    {
        errno = ENOSYS;
        return -1;
    }

    uint32_t arg_table[6] = {0};

    void* arg_ptr = (&no + 1); // parameter after no
    for (size_t i { 0 }; i < table[no].arg_cnt; ++i)
    {
        if (table[no].arg_sizes[i] == 1)
        {
            arg_table[i] = *((uint8_t*)arg_ptr);
        }
        else if (table[no].arg_sizes[i] == 2)
        {
            arg_table[i] = *((uint16_t*)arg_ptr);
        }
        else if (table[no].arg_sizes[i] == 4)
        {
            arg_table[i] = *((uint32_t*)arg_ptr);
        }

        arg_ptr = (uint8_t*)arg_ptr + table[no].arg_sizes[i];
    }

    if (type == 0)
    {
        return do_ludos_syscall(no, table[no].arg_cnt, arg_table);
    }
    else
    {
        return do_linux_syscall(no, table[no].arg_cnt, arg_table);
    }
}
