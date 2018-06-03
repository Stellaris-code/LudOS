/*
initrd.cpp

Copyright (c) 05 Yann BOUCHER (yann)

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

#include "initrd/initrd.hpp"

#include "utils/logging.hpp"
#include "utils/memutils.hpp"
#include "fs/tar/tar.hpp"
#include "drivers/storage/disk.hpp"

#include "fs/fsutils.hpp"
#include "fs/vfs.hpp"

bool install_initrd()
{
    auto initrd_disk = get_initrd_disk();
    if (initrd_disk)
    {
        auto fs = FileSystem::get_disk_fs(*initrd_disk);
        if (!fs)
        {
            err("Initrd is not valid filesystem\n");
            return false;
        }

        auto root = fs->root();
        if (!root)
        {
            err("Couldn't load initrd on disk %s\n", initrd_disk->drive_name().c_str());
            return false;
        }

        auto initrd = vfs::root->create("initrd", vfs::node::Directory);
        if (vfs::mount(root, initrd))
        {
            log(Info, "Mounted initrd\n");
            return true;
        }
    }

    return false;
}
