/*
basecommands.cpp

Copyright (c) 23 Yann BOUCHER (yann)

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

#include "basecommands.hpp"

#include "shell/shell.hpp"

#include "utils/env.hpp"
#include "utils/stlutils.hpp"

void install_base_commands(Shell &sh)
{
    sh.register_command(
    {"test", "a test command",
     "nothing to say here",
     [](const std::vector<std::string>&)
     {
         puts("Called test");
         return 0;
     }});

    sh.register_command(
    {"help", "print terminal help",
     "help <command>",
     [&sh](const std::vector<std::string>& args)
     {
         if (args.size() > 1)
         {
             sh.error("help must be called with zero or one argument\n");
             return -1;
         }

         auto commands = sh.commands();
         std::sort(commands.begin(), commands.end(), [](const Shell::Command& lhs, const Shell::Command& rhs)
         {
             return lhs.cmd < rhs.cmd;
         });

         if (args.empty())
         {
             puts("LudOS Shell help :");
             for (const auto& cmd : commands)
             {
                 kprintf("Command '%s' : \n", cmd.cmd.c_str());
                 kprintf("\t Desc : %s\n", cmd.description.c_str());
             }
         }
         else
         {
             auto it = std::find_if(commands.begin(), commands.end(), [&args](const Shell::Command& cmd)
             {
                 return cmd.cmd == args[0];
             });

             if (it != commands.end())
             {
                 kprintf("%s help : %s\n", args[0].c_str(), it->help.c_str());
                 return 0;
             }
         }
         return 0;
     }});

    sh.register_command(
    {"get", "get an environment variable",
     "Usage : get <var>",
     [&sh](const std::vector<std::string>& args)
     {
         if (args.size() > 1)
         {
             sh.error("'get' was given too many arguments\n");
             return -1;
         }
         else if (args.empty())
         {
             for (const auto& pair : kenv)
             {
                kprintf("%s : '%s'\n", pair.first.c_str(), pair.second.c_str());
             }
         }
         else
         {
             auto env = kgetenv(args[0]);
             std::string str = (env?*env:"");

             kprintf("%s : '%s'\n", args[0].c_str(), str.c_str());
         }
         return 0;
     }});

    sh.register_command(
    {"set", "set an environment variable",
     "Usage : set <var> <value>",
     [&sh](const std::vector<std::string>& args)
     {
         if (args.size() != 2)
         {
             sh.error("'set' needs 2 arguments\n");
             return -1;
         }

         ksetenv(args[0], args[1]);
         kprintf("%s : '%s'\n", args[0].c_str(), args[1].c_str());
         return 0;
     }});

    sh.register_command(
    {"echo", "echoes input",
     "Usage : echo <args>",
     [](const std::vector<std::string>& args)
     {
         std::string str = join(args, " ");
         kprintf("%s\n", str.c_str());
         return 0;
     }});
}
