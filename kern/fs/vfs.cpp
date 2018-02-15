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

#include <typeinfo.hpp>

std::unordered_set<void*> created_node_list;

namespace vfs
{

std::vector<std::reference_wrapper<node>> descriptors;
std::shared_ptr<vfs_root> root;

void init()
{
    root = std::make_shared<vfs_root>();

    log(Info, "VFS initialized.\n");
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
        auto v = cur_node->readdir();
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

bool mount(std::shared_ptr<vfs::node> node, std::string mountpoint)
{
    if (!node->is_dir()) mountpoint += "/" + node->name();

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
    point->vfs_children.back()->set_parent(point.get());
    return true;
}

void traverse(const vfs::node &node, size_t indent)
{
    if (node.name() != "." && node.name() != "..")
    {
        for (size_t i { 0 }; i < indent; ++i)
        {
            kprintf("\t");
        }

        if (indent > 0)
        {
            kprintf("└─");
        }
        kprintf("%s", node.name().c_str());
        if (node.is_dir())
        {
            kprintf("/");
        };
        kprintf("\n");
        if (node.is_dir())
        {
            for (const auto& entry : node.readdir())
            {
                vfs::traverse(*entry, indent+1);
            }
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

node::~node()
{
}

MemBuffer node::read(size_t offset, size_t size) const
{
    assert(!is_dir());
    assert(offset + size <= this->size());

    auto data = read_impl(offset, size);

    assert(data.size() == size);

    return data;
}

bool node::write(size_t offset, gsl::span<const uint8_t> data)
{
    assert(!is_dir());
    assert(offset + data.size() <= size());

    return write_impl(offset, data);
}

node *node::mkdir(const std::string & str)
{
    assert(is_dir());

    return mkdir_impl(str);
}

node *node::touch(const std::string & str)
{
    assert(is_dir());

    return touch_impl(str);
}

std::string node::path() const
{
    std::string suffix;
    if (is_dir()) suffix = "/";

    if (!m_parent)
    {
        return "" + suffix;
    }
    else
    {
        return m_parent->path() + name() + suffix;
    }
}

std::vector<std::shared_ptr<node> > node::readdir()
{
    assert(is_dir());

    static std::vector<std::shared_ptr<const node>> fkcghugelist;

    auto list = vfs_children;
    merge(list, readdir_impl());

    auto cur_dir = std::make_shared<symlink>(*this);
    cur_dir->rename(".");

    list.emplace_back(cur_dir);

    if (m_parent)
    {
        auto parent_dir = std::make_shared<symlink>(*m_parent);
        parent_dir->rename("..");

        list.emplace_back(parent_dir);
    }

    for (auto el : list)
    {
        fkcghugelist.emplace_back(el);
    }

    return list;
}

std::vector<std::shared_ptr<const node> > node::readdir() const
{
    static std::vector<std::shared_ptr<const node>> fkcghugelist;

    std::vector<std::shared_ptr<const node>> vec;
    for (auto el : const_cast<node*>(this)->readdir())
    {
        vec.emplace_back(el);
        fkcghugelist.emplace_back(el);
    }
    return vec;
}

bool is_symlink(const node &node)
{
    return dynamic_cast<const symlink*>(&node);
}

node &link_target(const node &link)
{
    return static_cast<const symlink&>(link).target();
}

}
