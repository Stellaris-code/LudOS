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

#include <errno.h>

#include <sys/fnctl.h>
#include <sys/fs.h>

#include "../shell.hpp"

#include "utils/env.hpp"
#include "../stlutils.hpp"

#include "syscalls/syscall_list.hpp"


void install_base_commands(Shell &sh)
{
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
                 printf("Command '%s' : \n", cmd.cmd.c_str());
                 printf("\t Desc : %s\n", cmd.description.c_str());
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
                 printf("%s help : %s\n", args[0].c_str(), it->help.c_str());
                 return 0;
             }
         }
         return 0;
     }});

//    sh.register_command(
//    {"get", "get an environment variable",
//     "Usage : get <var>",
//     [&sh](const std::vector<std::string>& args)
//     {
//         if (args.size() > 1)
//         {
//             sh.error("'get' was given too many arguments\n");
//             return -1;
//         }
//         else if (args.empty())
//         {
//             for (const auto& pair : kenv)
//             {
//                 printf("%s : '%s'\n", pair.first.c_str(), pair.second.c_str());
//             }
//         }
//         else
//         {
//             auto env = getenv(args[0]);
//             std::string str = (env?*env:"");

//             printf("%s : '%s'\n", args[0].c_str(), str.c_str());
//         }
//         return 0;
//     }});

//    sh.register_command(
//    {"set", "set an environment variable",
//     "Usage : set <var> <value>",
//     [&sh](const std::vector<std::string>& args)
//     {
//         if (args.size() != 2)
//         {
//             sh.error("'set' needs 2 arguments\n");
//             return -1;
//         }

//         setenv(args[0], args[1]);
//         return 0;
//     }});

    sh.register_command(
    {"echo", "echoes string",
     "Usage : echo <args>",
     [](const std::vector<std::string>& args)
     {
         std::string str;
         if (args.size() > 1 && args[0] == "-s")
         {
             str = args[1];
             print_serial(str.c_str());
         }
         else if (!args.empty())
         {
             str = args[0];
         }

         printf("%s\n", str.c_str());
         return 0;
     }});

    sh.register_command(
    {"settitle", "set terminal title",
     "Usage : settitle <title>",
     [](const std::vector<std::string>& args)
     {
         if (!args.empty())
         {
             //term().set_title(u8_decode(args[0]), term_data().title_color);
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

         int fd = open(sh.get_path(args[0]).c_str(), O_RDONLY, 0);
         if (fd < 0)
         {
             sh.error("Can't open file %s !\n", sh.get_path(args[0]).c_str());
             return -2;
         }

         off_t file_len = lseek(fd, 0, SEEK_END);
         if (file_len < 0)
         {
             sh.error("Error seeking end of file %s\n", args[0].c_str());
             return -3;
         }

         lseek(fd, 0, SEEK_SET);

         std::vector<uint8_t> data(file_len);
         int ret = read(fd, data.data(), file_len);
         if (ret < 0)
         {
             sh.error("Error reading file %s : %d (%s)\n", args[0].c_str(), errno, strerror(errno));
             return -4;
         }

         data.emplace_back('\0');

         const char* c_str = reinterpret_cast<const char*>(data.data());
         auto commands = tokenize(c_str, "\n");
         for (const auto& cmd : commands)
         {
             if (int rc = sh.command(std::string(cmd)); rc != 0)
             {
                 return rc;
             }
         }

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
         panic("Implement"); // TODO !!!
         for (auto& tok : toks) { tok = '"' + tok + '"'; }
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

         panic(reason.c_str());

         return 0;
     }});

    sh.register_command(
    {"greet", "Greets the user",
     "Usage : greet",
     [](const std::vector<std::string>&)
     {
         puts("Welcome to : \n");

         puts(
                 #include "ludos_art.txt"
             );

         return 0;
     }});

//    sh.register_command(
//    {"clear", "clears the screen",
//     "Usage : clear",
//     [](const std::vector<std::string>&)
//     {
//         for (size_t i { 0 }; i < term().height(); ++i)
//         {
//             putchar('\n');
//         }
//         return 0;
//     }});

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

         //cowsay(join(args, " "));
         return 0;
     }});

//    sh.register_command(
//    {"time", "benchmark command",
//     "Usage : 'time <command>'",
//     [&sh](const std::vector<std::string>& args)
//     {
//         std::string command = join(args, " ");

//         auto start = Time::uptime();
//         sh.command(command);
//         auto end = Time::uptime();

//         printf("Elapsed time : %f\n", end - start);
//         return 0;
//     }});

    sh.register_command(
    {"loop", "repeat command forever",
     "Usage : 'loop <command> <time>'",
     [&sh](const std::vector<std::string>& args)
     {
         while (true)
         {
             sh.command(join(args, " "));
         }
         return 0;
     }});

    sh.register_command(
    {"rep", "repeat command n times",
     "Usage : 'rep <time> <command>'",
     [&sh](const std::vector<std::string>& args)
     {
         if (args.size() < 2)
         {
             sh.error("not enough args");
             return -1;
         }

         size_t times = std::stoul(args[0]);
         for (size_t i { 0 }; i < times; ++i)
         {
             sh.command(join(gsl::span<const std::string>(args.data() + 1, args.size()-1), " "));
         }
         return 0;
     }});
}
