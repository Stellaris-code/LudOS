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

#include "shell/shell.hpp"
#include "drivers/diskinterface.hpp"
#include "utils/memutils.hpp"
#include "fs/vfs.hpp"

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
     "Usage : cat <file>",
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

         std::vector<uint8_t> data(node->size());
         if (!node->read(data.data(), data.size()))
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
         for (size_t i { 0 }; i < DiskInterface::drive_count(); ++i)
         {
             auto info = DiskInterface::info(i);
             kprintf("%s : %s\n", info.drive_name.c_str(), human_readable_size(info.disk_size).c_str());
         }
         return 0;
     }});
}
