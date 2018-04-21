/*
procfs.cpp

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

#include "procfs.hpp"

#include "fs/fsutils.hpp"
#include "fs/vfs.hpp"

#include "tasking/process.hpp"

#include "pid_node.hpp"
#include "fs/utils/string_node.hpp"
#include "time/time.hpp"

#include "info/cmdline.hpp"
#include "info/version.hpp"

#include "panic.hpp"

namespace procfs
{

struct procfs_root : public vfs::node
{
    using node::node;
public:
    virtual Type type() const override { return Directory; }
    virtual std::vector<std::shared_ptr<node>> readdir_impl() override
    {
        std::vector<std::shared_ptr<node>> children;

        for (auto pid : Process::process_list())
        {
            children.emplace_back(std::make_shared<pid_node>(pid));
        }

        children.emplace_back(std::make_shared<string_node>("cmdline", kernel_cmdline));
        children.emplace_back(std::make_shared<string_node>("uptime",  []{ return std::to_string(Time::uptime()); }));
        children.emplace_back(std::make_shared<string_node>("version", get_version_str()));
        if (Process::enabled()) children.emplace_back(std::make_shared<vfs::symlink>(std::to_string(Process::current().pid), "self"));

        return children;
    }
};

}

void procfs::init()
{
    auto dir = vfs::root->create("proc", vfs::node::Directory);
    if (!dir) panic("Could not create procfs !\n");

    auto root = std::make_shared<procfs_root>(vfs::root.get());

    if (!vfs::mount(root, dir))
    {
        panic("Couldn't mount procfs !\n");
    }
}
