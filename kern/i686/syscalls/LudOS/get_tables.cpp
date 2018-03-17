/*
get_tables.cpp

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

#include "i686/syscalls/syscall.hpp"
#include "syscalls/LudOS/syscalls.hpp"

#include "utils/logging.hpp"

void sys_get_syscall_tables(SyscallEntry *ludos, SyscallEntry *linux)
{
    for (size_t i { 0 }; i < max_syscalls; ++i)
    {
        ludos[i].arg_cnt = ludos_syscall_table[i].arg_cnt;
        memcpy(ludos[i].arg_sizes, ludos_syscall_table[i].arg_sizes, 6);

        linux[i].arg_cnt = linux_syscall_table[i].arg_cnt;
        memcpy(linux[i].arg_sizes, linux_syscall_table[i].arg_sizes, 6);
    }
}
