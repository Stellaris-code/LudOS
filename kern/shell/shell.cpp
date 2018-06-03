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

#include "unicode/utf8decoder.hpp"

#include "fs/vfs.hpp"
#include "drivers/kbd/kbd_mappings.hpp"

#include "cpp_runtime/exception_support.hpp"
#include "utils/demangle.hpp"

#include "halt.hpp"

Shell::Shell()
{
    m_command_history.emplace_back("");
    m_current_hist_idx = 0;

    pwd = vfs::root;

    m_input_handle = MessageBus::register_handler<TermInputEvent>([this](const TermInputEvent& e)
    {
        m_input = e.line;
        m_waiting_input = false;
    });

    m_key_handle   = MessageBus::register_handler<kbd::KeyEvent>([this](const kbd::KeyEvent& e)
    {
        if (e.state == kbd::KeyEvent::Pressed)
        {
            if (e.key == kbd::Up && m_waiting_input)
            {
                term().clear_input();

                ++m_current_hist_idx;

                if (m_current_hist_idx >= m_command_history.size())
                {
                    m_current_hist_idx = 0;
                }

                for (auto c : u8_decode(m_command_history[m_current_hist_idx]))
                {
                    term().add_input(c);
                }
                term().force_redraw_input();
            }
            if (e.key == kbd::Down && m_waiting_input)
            {
                term().clear_input();

                if (m_current_hist_idx == 0)
                {
                    m_current_hist_idx = m_command_history.size()-1;
                }
                else
                {
                    --m_current_hist_idx;
                }

                for (auto c : u8_decode(m_command_history[m_current_hist_idx]))
                {
                    term().add_input(c);
                }
                term().force_redraw_input();
            }
            if (e.key == kbd::Tab && m_waiting_input)
            {
                if (m_matches.empty())
                {
                    autocomplete();
                }
                else
                {
                    kpp::string input = term().input();
                    if (input.back() == '\t') input.pop_back();
                    auto toks = tokenize(input, " ", true);
                    auto actual_size = tokenize(input, " ").size();
                    if (!(toks.size() == 1 && actual_size != 1)) toks.pop_back();

                    ++m_current_match;
                    m_current_match %= m_matches.size();

                    kpp::string prefix = join(toks, " ");
                    if (toks.size() >= 1) prefix += " ";

                    term().set_input(prefix + m_matches[m_current_match]);
                }
            }
            else
            {
                m_matches.clear();
                m_current_match = 0;
            }
            update_coloring();
        }
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

        kpp::string input = read_input();
        process(input);
    }
}

int Shell::command(const kpp::string &command)
{
    auto tokens = quote_tokenize(command);

    if (tokens.empty())
    {
        return 0;
    }

    const auto cmd = tokens[0];
    auto args = std::vector<kpp::string>(tokens.begin() + 1, tokens.end());

    if (m_commands.find(cmd) != m_commands.end())
    {
        try
        {
            return m_commands[cmd].callback(args);
        }
        catch (const std::exception& e)
        {
            error("Command '%s' terminated with an exception of type '%s' :\n\t what : '%s'\n",
                  cmd.c_str(), demangle(current_exception_type().name()), e.what());
            return -2;
        }
    }

    error("Unknown command '%s'\n", cmd.c_str());
    return -1;
}

void Shell::show_prompt()
{
    kprintf("%s", prompt().c_str());
    term().switch_to_input();
    term().force_redraw();
}

kpp::string Shell::read_input()
{
    m_waiting_input = true;
    term().set_accept_input(true);
    while (m_waiting_input)
    {
        wait_for_interrupts();
    }
    term().set_accept_input(false);
    return m_input;
}

void Shell::process(const kpp::string &in)
{
    if (!in.empty())
    {
        m_command_history.emplace_front(in);
        m_current_hist_idx = m_command_history.size()-1;
    }

    int rc = command(in);
    if (rc != 0)
    {
        error("Command '%s' returned with exit code %d\n", in.c_str(), rc);
    }
}

void Shell::autocomplete()
{
    kpp::string input = term().input();

    if (input.substr(input.size()-term().tab_size, term().tab_size) == "    ")
    {
        input.erase(input.size()-term().tab_size, term().tab_size);
    }

    auto toks = tokenize(input, " ", true);
    auto actual_size = tokenize(input, " ").size();

    m_matches.clear();

    if (toks.size() == 1 && actual_size == 1)
    {
        for (const auto& pair : m_commands)
        {
            if (strtolower(pair.first).substr(0, toks.back().size()) == strtolower(toks.back()) && pair.first != toks.back())
            {
                m_matches.emplace_back(pair.first);
            }
        };
    }
    else
    {
        for (const auto& node : pwd->readdir())
        {
            if (toks.size() <= 1 || strtolower(node->name()).substr(0, toks.back().size()) == strtolower(toks.back()))
            {
                m_matches.emplace_back(node->name());
            }
        }
    }

    if (!m_matches.empty())
    {
        if (!(toks.size() == 1 && actual_size != 1) && !toks.empty()) toks.pop_back();

        kpp::string prefix = join(toks, " ");
        if (toks.size() >= 1) prefix += " ";

        term().set_input(prefix + m_matches[m_current_match]);
    }
}

kpp::string Shell::prompt() const
{
    return format(params.prompt, {{"path", pwd->path()}});
}

// TODO : coulouring : bad command/argument/quotes
void Shell::update_coloring()
{
    auto toks = quote_tokenize(term().input());

    kpp::string input = term().input();

    if (toks.empty()) return;

    auto cmd = toks[0];

    if (m_commands.find(cmd) != m_commands.end())
    {
        term().set_input_color(0, u8_decode(cmd).size(), {graphics::color_green, term_data().color().bg});
    }
    else
    {
        term().set_input_color(0, u8_decode(cmd).size(), {graphics::color_red, term_data().color().bg});
    }
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

kpp::string Shell::get_path(const kpp::string &path)
{
    if (path[0] == '/')
    {
        return path;
    }
    else
    {
        return pwd->path() + path;
    }
}

