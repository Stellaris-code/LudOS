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

#include "time/time.hpp"

#include "fs/fsutils.hpp"
#include "fs/vfs.hpp"

#include "panic.hpp"

void install_base_commands(Shell &sh)
{
    sh.register_command(
    {"test", "a test command",
     "nothing to say here",
     [](const std::vector<kpp::string>&)
     {
         puts("Called test");
         return 0;
     }});

    sh.register_command(
    {"help", "print terminal help",
     "help <command>",
     [&sh](const std::vector<kpp::string>& args)
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
     [&sh](const std::vector<kpp::string>& args)
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
             kpp::string str = (env?*env:"");

             kprintf("%s : '%s'\n", args[0].c_str(), str.c_str());
         }
         return 0;
     }});

    sh.register_command(
    {"set", "set an environment variable",
     "Usage : set <var> <value>",
     [&sh](const std::vector<kpp::string>& args)
     {
         if (args.size() != 2)
         {
             sh.error("'set' needs 2 arguments\n");
             return -1;
         }

         ksetenv(args[0], args[1]);
         return 0;
     }});

    sh.register_command(
    {"echo", "echoes string",
     "Usage : echo <args>",
     [](const std::vector<kpp::string>& args)
     {
         kpp::string str;
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
     [](const std::vector<kpp::string>& args)
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
     [](const std::vector<kpp::string>& args)
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
     [&sh](const std::vector<kpp::string>& args)
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

         auto result = file.value()->read();
         if (!result)
         {
             sh.error("Error reading file %s : %s\n", args[0].c_str(), result.error().to_string());
         }

         auto vec = std::move(result.value());

         if (vec.empty())
         {
             sh.error("Can't read file %s !\n", args[0].c_str());
             return -3;
         }
         vec.emplace_back('\0');

         auto old_pwd = sh.pwd;
         sh.pwd = std::shared_ptr<vfs::node>(file.value()->parent(), [](vfs::node*){});

         const char* c_str = reinterpret_cast<const char*>(vec.data());
         auto commands = tokenize(c_str, "\n");
         for (const auto& cmd : commands)
         {
             if (int rc = sh.command(cmd); rc != 0)
             {
                 sh.pwd = old_pwd;
                 return rc;
             }
         }

         sh.pwd = old_pwd;
         return 0;
     }});

    sh.register_command(
    {"alias", "creates an alias",
     "Usage : alias <name> <command>",
     [&sh](const std::vector<kpp::string>& args)
     {
         if (args.size() < 2)
         {
             sh.error("alias needs two arguments\n");
             return -1;
         }

         kpp::string alias = args[0];
         auto toks = std::vector<kpp::string>{args.begin() + 1, args.end()};
         panic("Implement"); // TODO !!!
         for (auto& tok : toks) { tok = '"' + tok + '"'; }
         kpp::string cmd = join(toks, " ");

         sh.register_command({alias, "<alias>", "<alias>", [&sh, cmd](const std::vector<kpp::string>&)
                              {sh.command(cmd);return 0;}});

         return 0;
     }});

    sh.register_command(
    {"panic", "forces a kernel panic",
     "Usage : panic (<reason>)",
     [](const std::vector<kpp::string>& args)
     {
         kpp::string reason = "Panic requested";
         if (!args.empty()) reason = join(args, " ");

         panic("%s\n", reason.c_str());

         return 0;
     }});

    sh.register_command(
    {"greet", "Greets the user",
     "Usage : greet",
     [](const std::vector<kpp::string>&)
     {
         greet();
         return 0;
     }});

    sh.register_command(
    {"clear", "clears the screen",
     "Usage : clear",
     [](const std::vector<kpp::string>&)
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
     [&sh](const std::vector<kpp::string>& args)
     {
         if (args.empty())
         {
             sh.error("'cowsay' nees at least one argument");
             return -1;
         }

         cowsay(join(args, " "));
         return 0;
     }});

    sh.register_command(
    {"time", "benchmark command",
     "Usage : 'time <command>'",
     [&sh](const std::vector<kpp::string>& args)
     {
         kpp::string command = join(args, " ");

         auto start = Time::uptime();
         sh.command(command);
         auto end = Time::uptime();

         kprintf("Elapsed time : %f\n", end - start);
         return 0;
     }});

    sh.register_command(
    {"loop", "loop command to infinity",
     "Usage : 'loop <command>'",
     [&sh](const std::vector<kpp::string>& args)
     {
         while (true)
         {
             sh.command(join(args, " "));
         }
         return 0;
     }});
}
