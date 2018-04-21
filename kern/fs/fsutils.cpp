/*
fsutils.cpp

Copyright (c) 13 Yann BOUCHER (yann)

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

#include "fsutils.hpp"

#include "tasking/process.hpp"
#include "tasking/process_data.hpp"

#include "vfs.hpp"
#include "pathutils.hpp"

namespace vfs
{

static constexpr size_t max_symlink_loop = 40;

bool is_symlink(const node &node)
{
    return dynamic_cast<const symlink*>(&node);
}

node &link_target(const node &link)
{
    return *vfs::find(static_cast<const symlink&>(link).target());
}

std::shared_ptr<vfs::node> find(const std::string& path, std::shared_ptr<node> search_root)
{
    if (path.empty())
    {
        return nullptr;
    }

    if (path == "/")
    {
        return search_root;
    }

    auto dirs = path_list(path);

    std::shared_ptr<vfs::node> cur_node = search_root;

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

std::shared_ptr<vfs::node> find(const std::string& path)
{
    return find(path, vfs::root);
}

QueryResult user_find_impl(const std::string &path, Process &process, size_t recur_depth)
{
    if (recur_depth >= max_symlink_loop) return {ELOOP, nullptr};
    if (path.empty()) return {ENOENT, nullptr};

    auto resolve_symlink_lambd = [&](std::shared_ptr<node> link, const std::string& pwd)->QueryResult
    {
        std::shared_ptr<vfs::node> cur_node = link;
        size_t counter = 0;
        while (cur_node->is_link())
        {
            if (counter++ >= max_symlink_loop) return {ELOOP, nullptr};

            auto sym_res = user_find_impl(pwd + std::static_pointer_cast<vfs::symlink>(cur_node)->target(), process, recur_depth + 1);
            if (sym_res.target_node == nullptr) return sym_res; // not found or invalid permissions
            cur_node = sym_res.target_node;
        }

        return {EOK, cur_node};
    };

    const bool relative = path.front() != '/';
    const bool is_dir   = path.back()  == '/';

    // split the search path into components
    auto dirs = path_list(path);

    // set the search root according to the relativeness of the pass
    std::shared_ptr<vfs::node> cur_node = relative ? process.data->pwd : process.data->root;
    std::string pwd = cur_node->path();

    for (size_t i { 0 }; i < dirs.size(); ++i)
    {
        // check at the beggining of the loop because we want to return the actual symlink at the end
        auto res = resolve_symlink_lambd(cur_node, pwd);
        if (res.target_node == nullptr) return res;
        cur_node = res.target_node;

        // now that symlink are resolved, update search pwd
        pwd += cur_node->name() + "/";

        // Check the current node for permissions and if it is a directory
        if (cur_node->type() != vfs::node::Directory) return {ENOTDIR, nullptr};
        if (!process.check_perms(cur_node->stat().perms, cur_node->stat().uid, cur_node->stat().gid,
                                 Process::ReadRequest|Process::ExecRequest))
        {
            return {EACCES, nullptr};
        }

        // iterate over the children of the current node
        for (const auto& child : cur_node->readdir())
        {
            if (child->name() == dirs[i])
            {
                cur_node = child;
                goto contin;
            }
        }
        // not found
        return {ENOENT, nullptr};
contin:;
    }

    // check if the final node is a dir if the path ended with a '/'
    if (is_dir)
    {
        auto res = resolve_symlink_lambd(cur_node, parent_path(pwd));
        if (res.target_node == nullptr) return res;
        cur_node = res.target_node;
        if (cur_node->type() != vfs::node::Directory)
        {
            return {ENOTDIR, nullptr};
        }
    }
    return {EOK, cur_node};
}

inline QueryResult user_find(const std::string &path, Process &process)
{
    return user_find_impl(path, process, 0);
}

QueryResult user_find(const std::string &path)
{
    return user_find(path, Process::current());
}

inline QueryResult resolve_symlink(const std::shared_ptr<node> link)
{
    std::shared_ptr<vfs::node> cur_node = link;
    size_t counter = 0;
    while (cur_node->is_link())
    {
        if (counter++ >= max_symlink_loop) return {ELOOP, nullptr};

        cur_node = find(cur_node->path() + std::static_pointer_cast<vfs::symlink>(cur_node)->target());
    }

    return {EOK, cur_node};
}

bool mount(std::shared_ptr<vfs::node> target, std::shared_ptr<vfs::node> mountpoint)
{
    if (target->type() != vfs::node::Directory)
    {
        return false;
    }

    mountpoint->m_mounted_node = target;
    target->set_parent(mountpoint.get());

    mounted_nodes.emplace_back(target);

    return true;
}

bool umount(std::shared_ptr<node> target)
{
    if (target->m_mounted_node == nullptr) return false;

    for (auto& ptr : mounted_nodes)
    {
        if (ptr.lock() == target->m_mounted_node) ptr.reset();
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

}
