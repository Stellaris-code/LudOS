/*
vfs.hpp

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
#ifndef VFS_HPP
#define VFS_HPP

#include <stdint.h>

#include <string.hpp>
#include <vector.hpp>
#include <functional.hpp>
#include <variant.hpp>
#include <optional.hpp>
#include <type_traits.hpp>
#include <unordered_map.hpp>

#include "utils/stlutils.hpp"
#include "utils/logging.hpp"
#include "panic.hpp"

namespace vfs
{
struct node
{
    virtual std::string name() const { return m_name; }
    virtual void rename(const std::string& name) { m_name = name; }
    virtual uint32_t permissions() const { return m_perms; }
    virtual void set_permissions(uint32_t perms) { m_perms = perms; }
    virtual uint32_t uid() const { return m_uid; }
    virtual void set_uid(uint32_t uid) { m_uid = uid; }
    virtual uint32_t gid() const { return m_gid; }
    virtual void set_gid(uint32_t gid) { m_gid = gid; }
    virtual uint32_t flags() const { return m_flags; }
    virtual void set_flags(uint32_t flags) { m_flags = flags; }

    virtual size_t read(void* buf, size_t n) const { return 0; }
    virtual size_t write(const void* buf, size_t n) { return 0; }
    virtual std::vector<std::shared_ptr<node>> readdir_impl() { return {}; }
    virtual node* mkdir(const std::string& str) { return nullptr; };
    virtual node* touch(const std::string& str) { return nullptr; }
    virtual size_t size() const { return 0; }
    virtual bool is_dir() const { return m_is_dir; }

    std::vector<std::shared_ptr<node>> readdir()
    {
        return merge(vfs_children, readdir_impl());
    }
    std::vector<std::shared_ptr<const node>> readdir() const
    {
        std::vector<std::shared_ptr<const node>> vec;
        for (const auto& el : const_cast<node*>(this)->readdir())
        {
            vec.emplace_back(el);
        }

        return vec;
    }

    std::vector<std::shared_ptr<node>> vfs_children {};

    uint32_t m_perms { 0 };
    uint32_t m_uid { 0 };
    uint32_t m_gid { 0 };
    uint32_t m_flags { 0 };

    std::string m_name {};
    bool m_is_dir { false };
};

size_t new_descriptor(node &node);

struct vfs_root : public node
{
    virtual std::string name() const override { return ""; }
    virtual node* mkdir(const std::string& str) override { panic("not implemented"); }
    virtual node* touch(const std::string& str) override { panic("not implemented"); }
    virtual bool is_dir() const override { return true; }
};

void init();

void mount_dev();

std::shared_ptr<node> find(const std::string& path);

bool mount(std::shared_ptr<node> node, const std::string& mountpoint);

void traverse(const vfs::node& node, size_t indent = 0);
void traverse(const std::string& path);

extern std::vector<std::reference_wrapper<node>> descriptors;
extern std::shared_ptr<vfs_root> root;
};

#endif // VFS_HPP
