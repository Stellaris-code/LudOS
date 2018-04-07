/*
openclose.cpp

Copyright (c) 20 Yann BOUCHER (yann)

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
#include "utils/user_ptr.hpp"

#include "fs/vfs.hpp"

int sys_open(user_ptr<const char> path, int flags, int mode)
{
    if (!path.check())
    {
        return -EFAULT;
    }

    auto node = vfs::find(path.get()); // TODO : pwd
    if (!node)
    {
        return -ENOENT;
    }

    if (flags & O_EXCL)
    {
        return -EEXIST;
    }

    tasking::FDInfo info;
    info.node = node;
    info.cursor = 0;

    const auto perms = node->stat().perms;
    if ((flags & 0b11) == O_RDONLY || (flags & 0b11) == O_RDWR)
    {
        if (!Process::current().check_perms(perms, node->stat().uid, node->stat().gid, Process::AccessRequestPerm::Read))
        {
            return -EACCES;
        }
        info.read = true;
    }
    if ((flags & 0b11) == O_WRONLY || (flags & 0b11) == O_RDWR)
    {
        if (!Process::current().check_perms(perms, node->stat().uid, node->stat().gid, Process::AccessRequestPerm::Write))
        {
            return -EACCES;
        }
        info.write = true;
    }

    if (flags & O_TRUNC)
    {
        // TODO :
        warn("O_TRUNC unsupported\n");
        return -ENOSYS;
    }
    if (flags & O_CREAT)
    {
        // TODO :
        warn("O_CREAT unsupported\n");
        return -ENOSYS;
    }
    if (flags & O_APPEND)
    {
        info.append = true;
    }

    int fd = Process::current().add_fd(info);

    return fd; // negative means everything is okay (yeah it's weird but it is for errno)
}

int sys_close(unsigned int fd)
{
    if (!Process::current().get_fd(fd))
    {
        return -EBADFD;
    }

    Process::current().close_fd(fd);

    return EOK;
}
