/*
readwrite.cpp

Copyright (c) 21 Yann BOUCHER (yann)

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

#include "syscalls/syscalls.hpp"

#include <errno.h>

extern int common_syscall(size_t type, size_t no, ...);

size_t read(unsigned int fd, void* buf, size_t count)
{
    auto ret = common_syscall(1, SYS_read, fd, buf, count);

    if (ret > 0)
    {
        errno = ret;
        return -1;
    }
    else
    {
        return -ret; // number of bytes read
    }
}


size_t write(unsigned int fd, const void* buf, size_t count)
{
    auto ret = common_syscall(1, SYS_write, fd, buf, count);

    if (ret > 0)
    {
        errno = ret;
        return -1;
    }
    else
    {
        return -ret; // number of bytes read
    }
}
