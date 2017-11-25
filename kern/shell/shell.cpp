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

#include "terminal/terminal.hpp"

#include "utils/messagebus.hpp"
#include "utils/stlutils.hpp"

#include "fs/vfs.hpp"

#include "halt.hpp"

Shell::Shell()
{
    MessageBus::register_handler<TermInputEvent>([this](const TermInputEvent& e)
    {
        m_input = e.line;
        m_waiting_input = false;
    });
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
    auto tokens = tokenize(command, " ", true);

    if (tokens.empty())
    {
        return 0;
    }

    const auto cmd = tokens[0];
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
    kprintf("%s", prompt().c_str());
    term().force_redraw();
}

std::string Shell::read_input()
{
    m_waiting_input = true;

    while (m_waiting_input)
    {
        wait_for_interrupts();
    }

    return m_input;
}

int Shell::process(const std::string &in)
{
    kprintf("%s\n", (prompt() + in).c_str());

    return command(in);
}

std::string Shell::prompt() const
{
    return format(params.prompt, {{"path", pwd}});
}

void Shell::error(const char *fmt, ...)
{
    va_list va;
    va_start(va, fmt);

    term_data().push_color({0xff0000, term_data().color().bg});
    kvprintf(fmt, va);
    term_data().pop_color();

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
