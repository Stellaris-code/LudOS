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

#include "syscalls/Linux/syscalls.hpp"

#include "tasking/process.hpp"
#include "sys/fnctl.h"
#include "errno.h"
#include "drivers/storage/disk.hpp"
#include "utils/user_ptr.hpp"

#include "fs/vfs.hpp"

size_t sys_read(unsigned int fd, user_ptr<void> buf, size_t count)
{
    if (!buf.check())
    {
        return -EFAULT;
    }

    auto fd_entry = Process::current().get_fd(fd);
    if (!fd_entry || !fd_entry->read)
    {
        return -EBADFD;
    }

    auto node = fd_entry->node;
    if (node->type() == vfs::node::Directory)
    {
        return -EISDIR;
    }

    if (node->size() && fd_entry->cursor + count >= node->size())
    {
        return -EIO;
    }

    MemBuffer data;

    data = node->read(fd_entry->cursor, count);
    // TODO : return -EIO

    if (data.empty() && count != 0)
    {
        return -EIO;
    }

    std::copy(data.begin(), data.end(), (uint8_t*)buf.get());

    return data.size(); // again, to allow errno numbers
}

size_t sys_write(unsigned int fd, user_ptr<const void> buf, size_t count)
{
    if (!buf.check())
    {
        return -EFAULT;
    }

    auto fd_entry = Process::current().get_fd(fd);
    if (!fd_entry || !fd_entry->write)
    {
        return -EBADFD;
    }

    auto node = fd_entry->node;
    if (node->type() == vfs::node::Directory)
    {
        return -EINVAL;
    }

    if (node->size() && fd_entry->cursor + count >= node->size())
    {
        return -EIO;
    }

    bool okay { false };

    okay = node->write(fd_entry->cursor, {(uint8_t*)buf.get(), (gsl::span<uint8_t>::index_type)(count)});
    // TODO : too

    if (!okay)
    {
        return -EIO;
    }

    return count; // again, to allow errno numbers
}

