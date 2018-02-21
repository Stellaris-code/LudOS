/*
fscommands.cpp

Copyright (c) 24 Yann BOUCHER (yann)

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

#include "fscommands.hpp"

#include "shell/commands/fs/ext2/ext2fscommands.hpp"

#include "shell/shell.hpp"
#include "drivers/storage/disk.hpp"
#include "utils/memutils.hpp"
#include "utils/crc32.hpp"
#include "fs/vfs.hpp"
#include "fs/fs.hpp"

void install_fs_commands(Shell &sh)
{
    sh.register_command(
    {"pwd", "print current directory",
     "Usage : pwd",
     [&sh](const std::vector<std::string>&)
     {
         puts(sh.pwd->path().c_str());
         return 0;
     }});

    sh.register_command(
    {"ls", "list current directory",
     "Usage : ls",
     [&sh](const std::vector<std::string>&)
     {
         for (const auto& entry : sh.pwd->readdir())
         {
             kprintf("\t%s", entry->name().c_str());
             if (entry->is_dir())
             {
                 kprintf("/");
             }
             else
             {
                 kprintf("\t: %s", human_readable_size(entry->size()).c_str());
             }
             kprintf("\n");
         }
         return 0;
     }});

    sh.register_command(
    {"cd", "change directory",
     "Usage : cd <dir>",
     [&sh](const std::vector<std::string>& args)
     {
         std::string target = "/";
         if (!args.empty()) target = args[0];

         auto node = vfs::find(sh.get_path(target));

         if (!node)
         {
             sh.error("Directory '%s' doesn't exist !\n", target.c_str());
             return -2;
         }
         if (!node->is_dir())
         {
             sh.error("'%s' is not a directory !\n", target.c_str());
             return -3;
         }

         if (vfs::is_symlink(*node))
         {
             vfs::node* current = node.get();
             while (vfs::is_symlink(*current))
             {
                 current = &vfs::link_target(*current);
             }

             sh.pwd = std::shared_ptr<vfs::node>(current, [](vfs::node*){});
         }
         else
         {
             sh.pwd = node;
         }
         return 0;
     }});

    sh.register_command(
    {"cat", "print file contents",
     "Usage : cat <file> (offset) (size)",
     [&sh](const std::vector<std::string>& args)
     {
         std::string path = "/";

         if (!args.empty())
         {
             path = args[0];
         }

         auto node = vfs::find(sh.get_path(path));
         if (!node)
         {
             sh.error("file not found : '%s'\n", path.c_str());
             return -2;
         }

         size_t offset = 0;
         size_t size = node->size();

         if (args.size() >= 2)
         {
             offset = std::stoul(args[1]);
         }
         if (args.size() >= 3)
         {
             size = std::stoul(args[2]);
         }

         auto data = node->read(offset, size);
         if (data.empty())
         {
             sh.error("cannot read file : '%s'\n", path.c_str());
             return -3;
         }
         data.emplace_back('\0');

         puts(reinterpret_cast<const char*>(data.data()));

         return 0;
     }});

    sh.register_command(
    {"tree", "print current fs tree",
     "Usage : tree",
     [&sh](const std::vector<std::string>&)
     {
         vfs::traverse(*sh.pwd);
         return 0;
     }});

    sh.register_command(
    {"lsblk", "list current drives",
     "Usage : lsblk",
     [](const std::vector<std::string>&)
     {
         for (auto disk : Disk::disks())
         {
             kprintf("%s : %s\n", disk.get().drive_name().c_str(), human_readable_size(disk.get().disk_size()).c_str());
         }
         return 0;
     }});

    sh.register_command(
    {"df", "list current file systems",
     "Usage : df",
     [](const std::vector<std::string>&)
     {
         for (FileSystem& fs : FileSystem::fs_list())
         {
             kprintf("%s on %s : %s/%s (%d%%)\n", fs.type().c_str(), fs.disk().drive_name().c_str(),
             human_readable_size(fs.total_size()-fs.free_size()).c_str(), human_readable_size(fs.total_size()).c_str(),
             100-int((double)fs.free_size()/fs.total_size()*100.));
         }
         return 0;
     }});

    sh.register_command(
    {"crc32", "print file crc32 checksum",
     "Usage : crc32 <file>",
     [&sh](const std::vector<std::string>& args)
     {
         std::string path = "/";

         if (!args.empty())
         {
             path = args[0];
         }

         auto node = vfs::find(sh.get_path(path));
         if (!node)
         {
             sh.error("file not found : '%s'\n", path.c_str());
             return -2;
         }

         size_t crc32;
         for (size_t i { 0 }; i < 600; ++i)
         {
             auto data = node->read();
             if (data.empty())
             {
                 sh.error("cannot read file : '%s'\n", path.c_str());
                 return -3;
             }

             if (i == 0) crc32 = crc(data.begin(), data.end());
             else
             {
                 size_t temp = crc(data.begin(), data.end());
                 if (temp != crc32)
                 {
                     kprintf("Error ! %d\n", i);
                 }
                 crc32 = temp;
             }
         }

         return 0;
     }});

    sh.register_command(
    {"dump", "dump file",
     "Usage : 'dump <file> (offset) (size)'",
     [&sh](const std::vector<std::string>& args)
     {
         std::string path = "/";

         if (!args.empty())
         {
             path = args[0];
         }

         auto node = vfs::find(sh.get_path(path));
         if (!node)
         {
             sh.error("file not found : '%s'\n", path.c_str());
             return -2;
         }

         size_t offset = 0;

         if (args.size() >= 2)
         {
             offset = std::stoul(args[1]);
         }

         size_t size = node->size() - offset;
         if (args.size() >= 3)
         {
             size = std::stoul(args[2]);
         }

         auto data = node->read(offset, size);
         if (data.empty())
         {
             sh.error("cannot read file : '%s'\n", path.c_str());
             return -3;
         }

         dump(data.data(), data.size());

         return 0;
     }});

    sh.register_command(
    {"write", "write to file",
     "Usage : 'write <file> <data> (offset)'",
     [&sh](const std::vector<std::string>& args)
     {
         std::string path = "/";

         if (args.size() < 2)
         {
             sh.error("'write' needs at least 2 arguments\n");
             return -1;
         }
         path = args[0];

         auto node = vfs::find(sh.get_path(path));
         if (!node)
         {
             sh.error("file not found : '%s'\n", path.c_str());
             return -2;
         }

         std::string data = args[1];

         size_t offset = 0;
         if (args.size() >= 3) offset = std::stoul(args[2]);

         if (!node->write(offset, {(uint8_t*)data.data(), (int)data.size()}))
         {
             sh.error("cannot write to file : '%s'\n", path.c_str());
             return -3;
         }

         return 0;
     }});


    sh.register_command(
    {"ren", "rename file file",
     "Usage : 'ren <old> <new>'",
     [&sh](const std::vector<std::string>& args)
     {
         std::string path = "/";

         if (args.size() != 2)
         {
             sh.error("'ren' needs 2 arguments\n");
             return -1;
         }
         path = args[0];

         auto node = vfs::find(sh.get_path(path));
         if (!node)
         {
             sh.error("file not found : '%s'\n", path.c_str());
             return -2;
         }

         try
         {
             node->rename(args[1]);
         }
         catch (const std::exception& e)
         {
             sh.error("Can't rename file '%s' to '%s' : %s\n", args[0].c_str(), args[1].c_str(), e.what());
             return -3;
         }

         return 0;
     }});

    sh.register_command(
    {"sync", "flush disks cache",
     "Usage : sync",
     [](const std::vector<std::string>&)
     {
         MessageBus::send(SyncDisksCache{});
         return 0;
     }});

    install_ext2fs_commands(sh);
}
