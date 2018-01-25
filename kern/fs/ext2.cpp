/*
ext2.cpp

Copyright (c) 19 Yann BOUCHER (yann)

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

#include "ext2.hpp"

#include "utils/memutils.hpp"

// TODO : système de cache wrapper avec invalidation

bool Ext2FS::accept(const Disk &disk)
{
    auto superblock = read_superblock(disk);
    if (!superblock) return false;

    log(Info, "Name : '%s'\n", superblock->last_mount_path);

    return superblock->ext2_signature == ext2_signature;
}

std::optional<const Ext2FS::Superblock> Ext2FS::read_superblock(const Disk &disk)
{
    if (disk.disk_size() < 1024*2)
    {
        return {};
    }

    auto data = disk.read(1024, 1024);
    return *reinterpret_cast<const Ext2FS::Superblock*>(data.data());
}

//ADD_FS(Ext2FS)
