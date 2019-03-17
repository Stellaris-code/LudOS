/*
shell.cpp

Copyright (c) 22 Yann BOUCHER (yann)

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

#include "shell.hpp"

#include <stdio.h>

#include "stlutils.hpp"
#include "syscalls/syscall_list.hpp"

#include "unicode/utf8decoder.hpp"

Shell::Shell()
{
    m_command_history.emplace_back("");
    m_current_hist_idx = 0;
}

void Shell::register_command(const Shell::Command &command)
{
    m_commands[command.cmd] = command;
}

void Shell::run()
{
    while (true)
    {
        show_prompt();

        std::string input = read_input();
        process(input);
    }
}

int Shell::command(const std::string &command)
{
    auto tokens = quote_tokenize(command);

    if (tokens.empty())
    {
        return 0;
    }

    const auto cmd = std::string(tokens[0]);
    auto args = std::vector<std::string>(tokens.begin() + 1, tokens.end());

    if (m_commands.find(cmd) != m_commands.end())
    {
        return m_commands[cmd].callback(args);
    }

    error("Unknown command '%s'\n", cmd.c_str());
    return -1;
}

void Shell::show_prompt()
{
    printf("%s", prompt().c_str());
}

void read_str(char* buf, size_t len)
{
    char c;
    while ((c = getchar()) != '\n' && --len != 0)
    {
        *buf++ = c;
    }
    *buf = '\0';
}

std::string Shell::read_input()
{
    char buf[64];
    read_str(buf, 64);
    return std::string(buf);
}

void Shell::process(const std::string &in)
{
    if (!in.empty())
    {
        m_command_history.emplace_front(in);
        m_current_hist_idx = m_command_history.size()-1;
    }

    int rc = command(in);
    if (rc != 0)
    {
        error("Command '%s' returned with exit code %d (0x%x)\n", in.c_str(), rc, rc);
    }
}

void Shell::autocomplete()
{
}

std::string Shell::prompt() const
{
    char cwd[512];
    getcwd(cwd, 512);
    return format(params.prompt, {{"path", cwd}});
}

// TODO : coulouring : bad command/argument/quotes
void Shell::update_coloring()
{
}

void Shell::error(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    char buffer[512];

    vsnprintf(buffer, 512, fmt, va);
    fprintf(stderr, "%s", buffer);

    va_end(va);
}

std::vector<Shell::Command> Shell::commands()
{
    std::vector<Command> vec;
    for (const auto& pair : m_commands)
    {
        vec.emplace_back(pair.second);
    }

    return vec;
}

std::string Shell::get_path(const std::string &path)
{
    if (path[0] == '/')
    {
        return path;
    }
    else
    {
        char cwd[512];
        getcwd(cwd, 512);
        return std::string(cwd) + path;
    }
}

