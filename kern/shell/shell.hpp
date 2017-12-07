/*
shell.hpp

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
#ifndef SHELL_HPP
#define SHELL_HPP

#include <deque.hpp>
#include <unordered_map.hpp>
#include <string.hpp>
#include <functional.hpp>

#include "utils/circularbuffer.hpp"
#include "utils/messagebus.hpp"

namespace vfs
{
class node;
}

class Shell
{
public:
    using CommandCallback = std::function<int(const std::vector<std::string>)>;
    struct Command
    {
        std::string cmd;
        std::string description;
        std::string help;
        CommandCallback callback;
    };

public:
    struct
    {
        std::string prompt;
    } params;

public:
    Shell();

    void register_command(const Command& command);

    void run();

    int command(const std::string& command);

    PRINTF_FMT(2, 3)
    void error(const char* fmt, ...);

    std::vector<Command> commands();

    std::string get_path(const std::string& path);

public:
    std::shared_ptr<vfs::node> pwd { nullptr };

private:
    void show_prompt();
    std::string read_input();
    void process(const std::string& in);
    void autocomplete();
    std::string prompt() const;
    void update_coloring();

private:
    std::unordered_map<std::string, Command> m_commands;
    volatile bool m_waiting_input { true };
    std::string m_input;

    std::deque<std::string> m_command_history;
    size_t m_current_hist_idx { 0 };

    std::vector<std::string> m_matches;
    size_t m_current_match { 0 };

    MessageBus::RAIIHandle m_input_handle;
    MessageBus::RAIIHandle m_key_handle;
};

#endif // SHELL_HPP
