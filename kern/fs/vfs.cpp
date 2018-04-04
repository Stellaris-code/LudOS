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
#include "time/time.hpp"
#include "power/powermanagement.hpp"
#include "drivers/storage/disk.hpp"

#include <typeinfo.hpp>

std::unordered_set<void*> created_node_list;

namespace vfs
{

std::vector<std::reference_wrapper<node>> descriptors;
std::shared_ptr<vfs_root> root;
std::vector<node*> mounted_nodes;

void init()
{
    root = std::make_shared<vfs_root>();

    MessageBus::register_handler<ShutdownMessage>([](const ShutdownMessage&)
    {
        for (auto ptr : mounted_nodes)
        {
            if (ptr) ptr->~node(); // force unmounting of mounted nodes
        }

        MessageBus::send(SyncDisksCache{});
    });

    log(Info, "VFS initialized.\n");
}

std::shared_ptr<vfs::node> find(const std::string& path)
{
    if (path.empty())
    {
        return nullptr;
    }

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

bool mount(std::shared_ptr<vfs::node> target, std::shared_ptr<vfs::node> mountpoint)
{
    if (target->type() != vfs::node::Directory)
    {
        return false;
    }

    mountpoint->m_mounted_node = target;
    target->set_parent(mountpoint.get());

    mounted_nodes.emplace_back(target.get());

    return true;
}

bool umount(std::shared_ptr<node> target)
{
    if (target->m_mounted_node == nullptr) return false;

    for (auto& ptr : mounted_nodes)
    {
        if (ptr == target->m_mounted_node.get()) ptr = nullptr;
    }

    target->m_mounted_node = nullptr;

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
        if (node.type() == vfs::node::Directory)
        {
            kprintf("/");
        };
        kprintf("\n");
        if (node.type() == vfs::node::Directory)
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

void node::rename(const std::string &name)
{
    // Check if no entries with the same name already exist
    if (parent())
    {
        for (auto node : parent()->readdir())
        {
            if (node->name() == name) return;
        }
    }

    m_name = name;
    rename_impl(name);

    update_modification_time();
}

MemBuffer node::read(size_t offset, size_t size) const
{
    assert(type() != Directory);
    if (this->size()) assert(offset + size <= this->size());

    auto data = read_impl(offset, size);

    assert(data.size() == size);

    update_access_time();

    return data;
}

bool node::write(size_t offset, gsl::span<const uint8_t> data)
{
    assert(type() != Directory);
    if (size()) assert(offset + data.size() <= size());

    auto result = write_impl(offset, data);
    update_modification_time();
    return result;
}

bool node::resize(size_t size)
{
    assert(type() != Directory);

    return resize_impl(size);
}

std::shared_ptr<node> node::create(const std::string & str, Type type)
{
    assert(this->type() == Directory);

    auto node = (m_mounted_node ? m_mounted_node->create(str, type) : create_impl(str, type));
    if (!node) return nullptr;

    update_modification_time();

    return node;
}

std::string node::path() const
{
    std::string suffix;
    if (type() == Directory) suffix = "/";

    if (!m_parent)
    {
        return "" + suffix;
    }
    else
    {
        return m_parent->path() + name() + suffix;
    }
}

void node::update_access_time() const
{
    auto stat = this->stat();
    stat.access_time = Time::epoch();
    const_cast<node*>(this)->set_stat(stat);
}

void node::update_modification_time()
{
    auto stat = this->stat();
    stat.modification_time = stat.access_time = Time::epoch();
    set_stat(stat);
}

node::Stat node::mkstat()
{
    Stat stat;
    stat.access_time = stat.creation_time = stat.modification_time = Time::epoch();
    stat.uid = stat.gid = 0;
    stat.perms = 0x0FFF;

    return stat;
}

std::vector<std::shared_ptr<node> > node::readdir()
{
    //assert(is_dir());

    static std::vector<std::shared_ptr<const node>> fkcghugelist;

    auto list = (m_mounted_node ? m_mounted_node->readdir_impl() : readdir_impl());

    auto cur_dir = std::make_shared<symlink>(*this);
    cur_dir->m_name = ".";

    list.emplace_back(cur_dir);

    if (m_parent)
    {
        auto parent_dir = std::make_shared<symlink>(*m_parent);
        parent_dir->m_name = "..";

        list.emplace_back(parent_dir);
    }

    for (auto el : list)
    {
        fkcghugelist.emplace_back(el);
    }

    update_access_time();

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

bool node::remove(const node *child)
{
    if (m_mounted_node) return m_mounted_node->remove_impl(child);
    return remove_impl(child);
}

bool is_symlink(const node &node)
{
    return dynamic_cast<const symlink*>(&node);
}

node &link_target(const node &link)
{
    return static_cast<const symlink&>(link).target();
}

std::shared_ptr<node> vfs_root::add_node(const std::string &name, Type type)
{
    m_children.emplace_back(std::make_shared<vfs::node>(this));
    auto node = m_children.back();

    node->m_name = name;
    node->m_type = type;
    node->m_stat = mkstat();

    return node;
}

bool vfs_root::remove_impl(const node *child)
{
    bool found = false;
    m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [child, &found](std::shared_ptr<node> node)
    {
        return found = (node.get() == child);
    }));

    return found;
}

}
