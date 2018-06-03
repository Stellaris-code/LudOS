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

#include <typeinfo.hpp>

#include "pathutils.hpp"

#include "utils/nop.hpp"
#include "utils/messagebus.hpp"
#include "utils/logging.hpp"

#include "fsutils.hpp"

#include "time/time.hpp"

#include "power/powermanagement.hpp"

#include "drivers/storage/disk.hpp"
#include "drivers/kbd/text_handler.hpp"

#include "tasking/process.hpp"

namespace vfs
{

std::shared_ptr<vfs_root> root;
std::vector<std::weak_ptr<node>> mounted_nodes;

void init()
{
    root = std::make_shared<vfs_root>();

    MessageBus::register_handler<ShutdownMessage>([](const ShutdownMessage&)
    {
        for (auto ptr : mounted_nodes)
        {
            if (!ptr.expired()) ptr.lock()->~node(); // force unmounting of mounted nodes
        }

        MessageBus::send(SyncDisksCache{});
    });

    log(Info, "VFS initialized.\n");
}

node::node(node *parent)
{
    set_parent(parent);
    m_stat = mkstat();
}

node::~node()
{
}

void node::rename(const kpp::string &name)
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

    //assert(data.size() == size);

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

std::shared_ptr<node> node::create(const kpp::string & str, Type type)
{
    assert(this->type() == Directory);

    auto node = (m_mounted_node ? m_mounted_node->create(str, type) : create_impl(str, type));
    if (!node) return nullptr;

    update_modification_time();

    return node;
}

kpp::string node::path() const
{
    kpp::string suffix;
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
    auto list = (m_mounted_node ? m_mounted_node->readdir_impl() : readdir_impl());

    auto cur_dir = std::make_shared<symlink>(path(), ".");

    list.emplace_back(cur_dir);

    if (m_parent)
    {
        auto parent_dir = std::make_shared<symlink>(m_parent->path(), "..");

        list.emplace_back(parent_dir);
    }

    update_access_time();

    return list;
}

std::vector<std::shared_ptr<const node> > node::readdir() const
{
    std::vector<std::shared_ptr<const node>> vec;

    for (auto el : const_cast<node*>(this)->readdir())
    {
        vec.emplace_back(el);
    }

    return vec;
}

bool node::remove(const node *child)
{
    if (m_mounted_node) return m_mounted_node->remove_impl(child);
    return remove_impl(child);
}

std::shared_ptr<node> vfs_root::add_node(const kpp::string &name, Type type)
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

symlink::symlink(kpp::string target)
    : m_target(target), m_linkname(filename(target))
{

}

symlink::symlink(kpp::string target, kpp::string name)
    : m_target(target), m_linkname(name)
{

}

kpp::string symlink::name() const
{
    return m_linkname;
}

std::shared_ptr<node> symlink::actual_target()
{
    return find(m_target).value_or(nullptr);
}

std::shared_ptr<const node> symlink::actual_target() const
{
    return find(m_target).value_or(nullptr);
}

}
