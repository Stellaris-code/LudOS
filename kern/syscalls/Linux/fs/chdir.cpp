/*
chdir.cpp

Copyright (c) 09 Yann BOUCHER (yann)

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

#include <errno.h>

#include "fs/fsutils.hpp"
#include "fs/vfs.hpp"
#include "tasking/process.hpp"
#include "tasking/process_data.hpp"
#include "utils/user_ptr.hpp"

static int chdir_impl(const std::shared_ptr<vfs::node>& node)
{
    auto link_result = vfs::resolve_symlink(node);
    if (link_result.target_node == nullptr)
    {
        return -link_result.error;
    }

    auto target = link_result.target_node;

    if (target->type() != vfs::node::Directory)
    {
        return -ENOTDIR;
    }

    Process::current().data->pwd = target;

    return EOK;
}

int sys_chdir(user_ptr<const char> path)
{
    if (!path.check())
    {
        return -EFAULT;
    }

    auto result = vfs::user_find(path.get());
    if (result.target_node == nullptr)
    {
        return -result.error;
    }

    return chdir_impl(result.target_node);
}


int sys_fchdir(int fd)
{
    auto fd_entry = Process::current().get_fd(fd);
    if (!fd_entry || !fd_entry->read)
    {
        return -EBADFD;
    }

    return chdir_impl(fd_entry->node);
}
