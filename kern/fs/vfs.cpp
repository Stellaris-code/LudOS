/*
vfs.cpp

Copyright (c) 17 Yann BOUCHER (yann)

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

#include "vfs.hpp"

#include "utils/logging.hpp"

#include "pathutils.hpp"
#include "utils/nop.hpp"
#include "utils/messagebus.hpp"
#include "drivers/kbd/text_handler.hpp"

#include "fat.hpp"

#include <typeinfo.hpp>

struct stdout_file : public vfs::node
{
    virtual size_t write(const void* buf, size_t n) override
    {
        auto str = std::string(reinterpret_cast<const char*>(buf), n);
        kprintf("%s", str.c_str());
        return n;
    }
};
struct stderr_file : public vfs::node
{
    virtual size_t write(const void* buf, size_t n) override
    {
        auto str = std::string(reinterpret_cast<const char*>(buf), n);

        err("%s", str.c_str());
        return n;
    }
};

struct stdin_file : public vfs::node
{
    virtual size_t read(void* data, size_t size) const override
    {
        std::vector<uint8_t> buf;

        auto handl = MessageBus::register_handler<kbd::TextEnteredEvent>([&buf](const kbd::TextEnteredEvent& e)
        {
            buf.push_back(e.c);
        });

        while (buf.size() < size) { nop(); };

        MessageBus::remove_handler(handl);

        for (size_t i { 0 }; i < buf.size(); ++i)
        {
            reinterpret_cast<uint8_t*>(data)[i] = buf[i];
        }

        return buf.size();
    }
};

namespace vfs
{

std::vector<std::reference_wrapper<node>> descriptors;
std::shared_ptr<vfs_root> root;

void init()
{
    root = std::make_shared<vfs_root>();

    log("VFS initialized.\n");
}

void mount_dev()
{
    root->vfs_children.emplace_back(std::make_shared<node>());
    root->vfs_children.back()->rename("dev");
    root->vfs_children.back()->m_is_dir = true;

    static auto stdin_node = std::make_shared<stdin_file>();
    stdin_node->rename("stdin");

    new_descriptor(*stdin_node);
    if (!mount(stdin_node, "/dev/stdin"))
    {
        warn("Can't mount '/dev/stdin'\n");
    }

    static auto stdout_node = std::make_shared<stdout_file>();
    stdout_node->rename("stdout");

    new_descriptor(*stdout_node);
    if (!mount(stdout_node, "/dev/stdout"))
    {
        warn("Can't mount '/dev/stdout'\n");
    }

    static auto stderr_node = std::make_shared<stderr_file>();
    stderr_node->rename("stderr");

    new_descriptor(*stderr_node);
    if (!mount(stderr_node, "/dev/stderr"))
    {
        warn("Can't mount '/dev/stderr'\n");
    }
}

std::shared_ptr<vfs::node> find(const std::string& path)
{
    if (path == "/")
    {
        return root;
    }

    auto dirs = path_list(path);

    std::shared_ptr<vfs::node> cur_node = root;

    for (size_t i { 0 }; i < dirs.size(); ++i)
    {
        const auto v = cur_node->readdir();
        for (const auto& child : v)
        {
            if (child->name() == dirs[i])
            {
                cur_node = child;
                goto contin;
            }
        }
        return {};

contin:;
    }

    return cur_node;
}

bool mount(std::shared_ptr<vfs::node> node, const std::string &mountpoint)
{
    auto point = find(parent_path(mountpoint));
    if (!point)
    {
        err("Mountpoint root '%s' doesn't exist\n", parent_path(mountpoint).c_str());
        return false;
    }
    if (find(mountpoint))
    {
        err("Mountpoint '%s' exists\n", mountpoint.c_str());
        return false;
    }
    point->vfs_children.emplace_back(node);
    point->vfs_children.back()->rename(filename(mountpoint));
    return true;
}

void traverse(const vfs::node &node, size_t indent)
{
    for (size_t i { 0 }; i < indent; ++i)
    {
        kprintf("\t");
    }

    if (indent > 0)
    {
        kprintf("\xc0\xc4");
    }
    kprintf("%s", node.name().c_str());
    if (node.is_dir())
    {
        kprintf("/");
    }
    kprintf("\n");
    if (node.is_dir())
    {
        for (const auto& entry : node.readdir())
        {
            vfs::traverse(*entry, indent+1);
        }
    }
}

void traverse(const std::string &path)
{
    if (!vfs::find(path))
    {
        err("Can't find '%s' !\n", path.c_str());
        return;
    }
    else
    {
        vfs::traverse(*vfs::find(path));
    }
}

size_t new_descriptor(vfs::node &node)
{
    descriptors.emplace_back(node);
    return descriptors.size()-1;
}

}
