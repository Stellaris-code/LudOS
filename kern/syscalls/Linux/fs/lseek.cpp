/*
lseek.cpp

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

#include "syscalls/Linux/syscalls.hpp"

#include "tasking/process.hpp"

#include "errno.h"
#include "sys/fs.h"
#include "fs/vfs.hpp"

int sys_lseek(unsigned int fd, int off, int whence)
{
    auto entry = Process::current().get_fd(fd);
    if (!entry)
    {
        return EBADFD;
    }

    if (whence != SEEK_CUR && whence != SEEK_SET && whence != SEEK_END)
    {
        return EINVAL;
    }

    int new_cursor = entry->cursor;

    if (whence == SEEK_CUR)
    {
        new_cursor += off;
    }
    else if (whence == SEEK_SET)
    {
        new_cursor = off;
    }
    else if (whence == SEEK_END)
    {
        new_cursor = entry->node->size() + off;
    }

    if (new_cursor < 0 || new_cursor >= (int)entry->node->size())
    {
        return EINVAL;
    }

    entry->cursor = new_cursor;

    return -entry->cursor;
}
