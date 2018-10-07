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
#include "fs/interface.hpp"

#include <sys/interface_list.h>

#include "tasking/process.hpp"

#include "pid_node.hpp"
#include "fs/utils/string_node.hpp"
#include "time/time.hpp"

#include "info/cmdline.hpp"
#include "info/version.hpp"

#include "panic.hpp"

namespace procfs
{

struct interface_test : public vfs::interface_node<interface_test, vfs::ientry<itest, ITEST_ID>>
{
    interface_test(const kpp::string& name)
    {
        the_name = name;
    }

    virtual kpp::string name() const override
    { return the_name; }

    template<typename Interface>
    void fill_interface(Interface*) const
    {}

    int test_function(const char* str) const
    {
        log_serial("Called! : %s\n", str);
        return 42;
    }

    kpp::string the_name;
};
template <>
void interface_test::fill_interface<itest>(itest* interface) const
{
    register_callback(&interface_test::test_function, interface->test);
}

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
        children.emplace_back(std::make_shared<string_node>("uptime",  []{ return kpp::to_string(Time::uptime()); }));
        children.emplace_back(std::make_shared<string_node>("version", get_version_str()));
        children.emplace_back(std::make_shared<vfs::symlink>(kpp::to_string(Process::current().pid), "self"));

        children.emplace_back(std::make_shared<interface_test>("interface_test"));

        return children;
    }
};

}

void procfs::init()
{
    auto result = vfs::root->create("proc", vfs::node::Directory);
    if (!result) panic("Could not create procfs : %s\n", result.error().to_string());

    auto root = std::make_shared<procfs_root>(vfs::root.get());

    if (!vfs::mount(root, result.value()))
    {
        panic("Couldn't mount procfs !\n");
    }
}
