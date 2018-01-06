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

#include "misc/greet.hpp"
#include "misc/cowsay.hpp"

#include "terminal/terminal.hpp"

#include "fs/vfs.hpp"

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
    {"echo", "echoes string",
     "Usage : echo <args>",
     [](const std::vector<std::string>& args)
     {
         std::string str;
         if (args.size() > 1 && args[0] == "-s")
         {
             str = args[1];
             log_serial("%s\n", str.c_str());
         }
         else if (!args.empty())
         {
             str = args[0];
         }

         kprintf("%s\n", str.c_str());
         return 0;
     }});

    sh.register_command(
    {"settitle", "set terminal title",
     "Usage : settitle <title>",
     [](const std::vector<std::string>& args)
     {
         if (!args.empty())
         {
             term().set_title(u8_decode(args[0]), term_data().title_color);
         }

         return 0;
     }});

    sh.register_command(
    {"serial", "enable echoing to serial port",
     "Usage : serial <on/off>",
     [](const std::vector<std::string>& args)
     {
         if (!args.empty())
         {
             if (args[0] == "on")
             {
                 putc_serial = true;
                 puts("Echoing to serial port");
             }
             else if (args[0] == "off")
             {
                 putc_serial = false;
                 puts("Serial port disabled");
             }
             else
             {
                 return -1;
             }
         }
         return 0;
     }});

    sh.register_command(
    {"run", "run shell script",
     "Usage : run <file>",
     [&sh](const std::vector<std::string>& args)
     {
         if (args.size() != 1)
         {
             sh.error("'run' needs one argument !\n");
             return -1;
         }

         auto file = vfs::find(sh.get_path(args[0]));
         if (!file)
         {
             sh.error("Can't open file %s !\n", args[0].c_str());
             return -2;
         }

         std::vector<uint8_t> vec(file->size());
         if (!file->read(vec.data(), vec.size()))
         {
             sh.error("Can't read file %s !\n", args[0].c_str());
             return -3;
         }
         vec.emplace_back('\0');

         auto old_pwd = sh.pwd;
         sh.pwd = std::shared_ptr<vfs::node>(file->parent(), [](vfs::node*){});

         const char* c_str = reinterpret_cast<const char*>(vec.data());
         auto commands = tokenize(c_str, "\n");
         for (const auto& cmd : commands)
         {
             if (int rc = sh.command(cmd); rc != 0)
             {
                 return rc;
             }
         }

         sh.pwd = old_pwd;

         return 0;
     }});

    sh.register_command(
    {"alias", "creates an alias",
     "Usage : alias <name> <command>",
     [&sh](const std::vector<std::string>& args)
     {
         if (args.size() < 2)
         {
             sh.error("alias needs two arguments\n");
             return -1;
         }

         std::string alias = args[0];
         auto toks = std::vector<std::string>{args.begin() + 1, args.end()};
         for (auto& tok : toks) { tok.insert(tok.begin(), '"'); tok += '"'; }
         std::string cmd = join(toks, " ");

         sh.register_command({alias, "<alias>", "<alias>", [&sh, cmd](const std::vector<std::string>&)
                              {sh.command(cmd);return 0;}});

         return 0;
     }});

    sh.register_command(
    {"panic", "forces a kernel panic",
     "Usage : panic (<reason>)",
     [](const std::vector<std::string>& args)
     {
         std::string reason = "Panic requested";
         if (!args.empty()) reason = join(args, " ");

         panic("%s\n", reason.c_str());

         return 0;
     }});

    sh.register_command(
    {"greet", "Greets the user",
     "Usage : greet",
     [](const std::vector<std::string>&)
     {
         greet();
         return 0;
     }});

    sh.register_command(
    {"clear", "clears the screen",
     "Usage : clear",
     [](const std::vector<std::string>&)
     {
         for (size_t i { 0 }; i < term().height(); ++i)
         {
             putchar('\n');
         }
         return 0;
     }});

    sh.register_command(
    {"cowsay", "Let the cow speak",
     "Usage : cowsay <txt>",
     [&sh](const std::vector<std::string>& args)
     {
         if (args.empty())
         {
             sh.error("'cowsay' nees at least one argument");
             return -1;
         }

         cowsay(join(args, " "));
         return 0;
     }});
}
