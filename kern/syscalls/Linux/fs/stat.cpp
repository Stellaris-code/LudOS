/*
stat.cpp

Copyright (c) 12 Yann BOUCHER (yann)

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

#include "utils/user_ptr.hpp"

#include <sys/time.h>
#include <sys/stat.h>
#include <errno.h>

#include "fs/fsutils.hpp"
#include "fs/vfs.hpp"
#include "fs/fs.hpp"
#include "tasking/process.hpp"

int sys_stat(user_ptr<const char> path, user_ptr<struct stat> ptr)
{
    if (!path.check() || !ptr.check())
    {
        return -EFAULT;
    }

    auto result = vfs::user_find(path.get());
    if (result.target_node == nullptr)
    {
        return -result.error;
    }

    auto node_stat = result.target_node->stat();

    if (!Process::current().check_perms(node_stat.perms, node_stat.uid, node_stat.gid, Process::AccessRequestPerm::ReadRequest))
    {
        return -EPERM;
    }

    auto stat = ptr.get();
    // TODO : dev/ino
    const auto* fs = result.target_node->get_fs();
    stat->st_dev = fs ? fs->fs_id : 0;
    stat->st_ino = node_stat.inode;
    stat->st_mode = node_stat.perms;
    stat->st_nlink = node_stat.nlinks;
    stat->st_uid = node_stat.uid;
    stat->st_gid = node_stat.gid;
    stat->st_rdev = 0;
    stat->st_size = result.target_node->size();
    stat->st_blksize = node_stat.block_size;
    stat->st_blocks = node_stat.block_count;
    //stat->st_blocks = result.target_node->size()/512 + (result.target_node->size()%512?:0);


    switch (result.target_node->type())
    {
        case vfs::node::Type::Unknown:
        case vfs::node::Type::File:
            stat->st_mode |= S_IFREG;
        case vfs::node::Type::Directory:
            stat->st_mode |= S_IFDIR;
        case vfs::node::Type::SymLink:
            stat->st_mode |= S_IFLNK;
        case vfs::node::Type::FIFO:
            stat->st_mode |= S_IFIFO;
    }

    return EOK;
}
