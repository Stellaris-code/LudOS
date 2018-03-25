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
#ifndef i686_SYSCALL_HPP
#define i686_SYSCALL_HPP

#include "syscalls/syscalls.hpp"

#include <functional.hpp>

#include "i686/cpu/registers.hpp"

extern "C" uint32_t syscall_handler(const registers * const regs);

using syscall_ptr = std::function<uint32_t(const registers* const)>;

constexpr size_t max_syscalls { 1024 };

constexpr uint8_t invalid_syscall_magic { 0xFF };

struct SyscallInfo
{
    syscall_ptr ptr;
    uint8_t arg_cnt;
    uint8_t arg_sizes[6];
}; // 32 bytes

extern SyscallInfo ludos_syscall_table[max_syscalls];
extern SyscallInfo linux_syscall_table[max_syscalls];

#endif // i686_SYSCALL_HPP
