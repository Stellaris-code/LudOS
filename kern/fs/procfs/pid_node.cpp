/*
pid_node.cpp

Copyright (c) 11 Yann BOUCHER (yann)

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

#include "pid_node.hpp"

#include "tasking/process.hpp"
#include "tasking/process_data.hpp"
#include "fs/utils/string_node.hpp"

namespace procfs
{

kpp::string pid_node::name() const { return kpp::to_string(m_pid).c_str(); }

std::vector<std::shared_ptr<vfs::node> > pid_node::readdir_impl()
{
    std::vector<std::shared_ptr<node>> children;

    children.emplace_back(std::make_shared<string_node>("name", Process::by_pid(m_pid)->data->name));
    children.emplace_back(std::make_shared<vfs::symlink>(Process::by_pid(m_pid)->data->pwd->path(), "cwd"));
    children.emplace_back(std::make_shared<vfs::symlink>(Process::by_pid(m_pid)->data->root->path(), "root"));
    children.emplace_back(std::make_shared<string_node>("cmdline", [this]{
        kpp::string str;
        for (const auto& arg : Process::by_pid(m_pid)->data->args)
        {
            str += arg;
            str += '\0';
        }
        return str;
    }));

    return children;
}

}
