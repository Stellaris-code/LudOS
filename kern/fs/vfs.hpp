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
#include <unordered_set.hpp>
#include <utils/gsl/gsl_span.hpp>

#include "utils/stlutils.hpp"
#include "utils/logging.hpp"
#include "utils/membuffer.hpp"
#include "panic.hpp"

extern std::unordered_set<void*> created_node_list;

namespace vfs
{
struct node
{
    friend struct symlink;

    node(node* parent = nullptr)
    {
        set_parent(parent);
    }

    node(const node&) = delete;
    node(node&&) = default;

    virtual ~node();

    virtual std::string name() const { return m_name; }
    void rename(const std::string& name);
    virtual uint32_t permissions() const { return m_perms; }
    virtual void set_permissions(uint32_t perms) { m_perms = perms; }
    virtual uint32_t uid() const { return m_uid; }
    virtual void set_uid(uint32_t uid) { m_uid = uid; }
    virtual uint32_t gid() const { return m_gid; }
    virtual void set_gid(uint32_t gid) { m_gid = gid; }
    virtual uint32_t flags() const { return m_flags; }
    virtual void set_flags(uint32_t flags) { m_flags = flags; }

    virtual size_t size() const { return 0; }
    virtual bool is_dir() const { return m_is_dir; }

    [[nodiscard]] MemBuffer read(size_t offset, size_t size) const;
    [[nodiscard]] MemBuffer read() const { return read(0, size()); }
    [[nodiscard]] bool write(size_t offset, gsl::span<const uint8_t> data);
    [[nodiscard]] node* mkdir(const std::string&);
    [[nodiscard]] node* touch(const std::string&);
    std::vector<std::shared_ptr<node>> readdir();
    std::vector<std::shared_ptr<const node>> readdir() const;
    void remove(const vfs::node* child);

    node* parent() const { return m_parent; }
    void set_parent(node* parent) { m_parent = parent; }

    std::string path() const;

    std::vector<std::shared_ptr<node>> vfs_children {};
    bool m_is_dir { false };

protected:
    [[nodiscard]] virtual MemBuffer read_impl(size_t, size_t) const { return {}; }
    [[nodiscard]] virtual bool write_impl(size_t, gsl::span<const uint8_t>) { return false; }
    virtual std::vector<std::shared_ptr<node>> readdir_impl() { return {}; }
    [[nodiscard]] virtual node* mkdir_impl(const std::string&) { return nullptr; }
    [[nodiscard]] virtual node* touch_impl(const std::string&) { return nullptr; }
    virtual void rename_impl(const std::string&) {}
    virtual void remove_impl(const vfs::node* child) {}

    uint32_t m_perms { 0 };
    uint32_t m_uid { 0 };
    uint32_t m_gid { 0 };
    uint32_t m_flags { 0 };

    std::string m_name {};

    node* m_parent { nullptr };
};

size_t new_descriptor(node &node);

struct vfs_root : public node
{
    vfs_root() : node(nullptr) {m_is_dir = true;}

    virtual std::string name() const override { return ""; }
    virtual bool is_dir() const override { return true; }

protected:
    virtual node* mkdir_impl(const std::string&) override { panic("not implemented"); }
    virtual node* touch_impl(const std::string&) override { panic("not implemented"); }
};

struct symlink : public node
{

    symlink(node& target)
        : m_target(target)
    {

    }

    virtual size_t size() const override { return m_target.size(); }
    virtual bool is_dir() const override { return m_target.is_dir(); }

    node& target() const { return m_target; }

protected:

    [[nodiscard]] virtual MemBuffer read_impl(size_t offset, size_t size) const override { return m_target.read(offset, size); }
    [[nodiscard]] virtual bool write_impl(size_t offset, gsl::span<const uint8_t> data) override { return m_target.write(offset, data); }
    virtual std::vector<std::shared_ptr<node>> readdir_impl() override { return m_target.readdir_impl(); }
    [[nodiscard]] virtual node* mkdir_impl(const std::string& s) override { return m_target.mkdir(s); };
    [[nodiscard]] virtual node* touch_impl(const std::string& s) override { return m_target.touch(s); }

private:
    node& m_target;
};

void init();

std::shared_ptr<node> find(const std::string& path);

bool mount(std::shared_ptr<node> node, std::string mountpoint);

void traverse(const vfs::node& node, size_t indent = 0);
void traverse(const std::string& path);

bool is_symlink(const vfs::node& node);
node& link_target(const vfs::node& link);

extern std::vector<std::reference_wrapper<node>> descriptors;
extern std::shared_ptr<vfs_root> root;
};

#endif // VFS_HPP
