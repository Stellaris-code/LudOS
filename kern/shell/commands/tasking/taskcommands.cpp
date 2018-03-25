/*
taskcommands.cpp

Copyright (c) 10 Yann BOUCHER (yann)

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

#include "taskcommands.hpp"

#include "shell/shell.hpp"

#include "fs/vfs.hpp"
#include "tasking/process.hpp"
#include "tasking/loaders/process_loader.hpp"

void install_task_commands(Shell &sh)
{
    sh.register_command(
    {"exec", "loads a process from a file and executes it",
     "Usage : exec <file>",
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

         auto buffer = node->read();
         auto loader = ProcessLoader::get(buffer);
         if (!loader)
         {
             sh.error("File '%s' is not in an executable format\n", args[0].c_str());
             return -3;
         }

         kprintf("File type : %s\n", loader->file_type().c_str());

         Process process;
         if (!loader->load(process))
         {
             sh.error("Can't load '%s'\n", args[0].c_str());
             return -4;
         }

         std::vector<std::string> program_args(args.size()+1);
         program_args[0] = sh.get_path(path);
         for (size_t i { 1 }; i < args.size(); ++i)
         {
             program_args[i] = args[i];
         }

         process.execute(program_args);

         return 0;
     }});
}
