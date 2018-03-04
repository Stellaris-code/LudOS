/*
ext2fscommands.hpp

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

#include "shell/commands/fs/fscommands.hpp"

#include "shell/shell.hpp"

#include "fs/vfs.hpp"
#include "fs/ext2/ext2.hpp"

#include "utils/memutils.hpp"

void install_ext2fs_commands(Shell& sh)
{
    sh.register_command(
    {"ext2info", "print file ext2 info",
     "Usage : ext2info <file>",
     [&sh](const std::vector<std::string>& args)
     {
         if (args.empty())
         {
             sh.error("'ext2info' needs one argument !\n");
             return -1;
         }

         std::string path = args[0];

         auto node = vfs::find(sh.get_path(path));
         if (!node)
         {
             sh.error("file not found : '%s'\n", path.c_str());
             return -2;
         }

         ext2_node* ext2 = dynamic_cast<ext2_node*>(node.get());
         if (!ext2)
         {
             sh.error("'%s' is not on an ext2 file system !\n", path.c_str());
             return -3;
         }

         auto inode_struct = ext2->fs.read_inode(ext2->inode);

         auto type = (ext2::InodeType)(inode_struct.type & 0xF000);
         std::string type_str = "other";
         switch (type)
         {
             case ext2::InodeType::Regular:
             type_str = "regular file";
             break;

             case ext2::InodeType::Directory:
             type_str = "directory";
             break;

             case ext2::InodeType::Symlink:
             type_str = "symlink";
             break;

             default:;
         }

         kprintf("File '%s' :\n", args[0].c_str());
         kprintf("\t Inode : %zd\n", ext2->inode);

         kprintf("\t Type : %s (0x%x)\n", type_str.c_str(), inode_struct.type);
         kprintf("\t Byte size : %d\n", inode_struct.size_lower);
         kprintf("\t 512-byte blocks : %d\n", inode_struct.blocks_512);
         kprintf("\t Links : %d\n", inode_struct.links_count);
         kprintf("\t Flags : 0x%x\n", inode_struct.flags);

         for (size_t i { 0 }; i < 15; ++i)
         {
             kprintf("\t Block %d : %d\n", i, inode_struct.block_ptr[i]);
         }

         return 0;
     }});
}
