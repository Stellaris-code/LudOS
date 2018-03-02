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
enum Permissions : uint16_t
{
    SUID = 0x0800,
    SGID = 0x0400,
    StickyBit = 0x0200,
    UserRead = 0x0100,
    UserWrite = 0x0080,
    UserExec = 0x0040,
    GroupRead = 0x0020,
    GroupWrite = 0x0010,
    GroupExec = 0x0008,
    OtherRead = 0x0004,
    OtherWrite = 0x0002,
    OtherExec = 0x0001
};

struct node
{
    friend struct symlink;
    friend struct vfs_root;
    friend bool mount(std::shared_ptr<node> target, std::shared_ptr<node> mountpoint);
    friend bool umount(std::shared_ptr<node> target);


    struct Stat
    {
        uint16_t perms { 0 };
        uint16_t uid { 0 };
        uint16_t gid { 0 };
        uint32_t flags { 0 };
        size_t access_time { 0 };
        size_t creation_time { 0 };
        size_t modification_time { 0 };
    };

    node(node* parent = nullptr)
    {
        set_parent(parent);
        m_stat = mkstat();
    }

    node(const node&) = delete;
    node(node&&) = default;

    virtual ~node();

    virtual std::string name() const { return m_name; }
    void rename(const std::string& name);

    virtual Stat stat() const { return m_stat; }
    virtual void set_stat(const Stat& stat) { m_stat = stat; }

    virtual size_t size() const { return 0; }
    virtual bool is_dir() const { return m_is_dir; }

    virtual bool is_link() const { return false; }

    [[nodiscard]] MemBuffer read(size_t offset, size_t size) const;
    [[nodiscard]] MemBuffer read() const { return read(0, size()); }
    [[nodiscard]] bool write(size_t offset, gsl::span<const uint8_t> data);
    [[nodiscard]] std::shared_ptr<node> mkdir(const std::string&);
    [[nodiscard]] std::shared_ptr<node> touch(const std::string&);
    std::vector<std::shared_ptr<node>> readdir();
    std::vector<std::shared_ptr<const node>> readdir() const;
    bool remove(const vfs::node* child);

    node* parent() const { return m_parent; }
    void set_parent(node* parent) { m_parent = parent; }

    std::string path() const;

protected:
    [[nodiscard]] virtual MemBuffer read_impl(size_t, size_t) const { return {}; }
    [[nodiscard]] virtual bool write_impl(size_t, gsl::span<const uint8_t>) { return false; }
    virtual std::vector<std::shared_ptr<node>> readdir_impl() { return {}; }
    [[nodiscard]] virtual std::shared_ptr<node> mkdir_impl(const std::string&) { return nullptr; }
    [[nodiscard]] virtual std::shared_ptr<node> touch_impl(const std::string&) { return nullptr; }
    virtual void rename_impl(const std::string&) {}
    virtual bool remove_impl(const vfs::node*) { return false; }

private:
    void update_access_time() const;
    void update_modification_time();

    static Stat mkstat();

protected:

    mutable Stat m_stat;

    std::string m_name {};

    node* m_parent { nullptr };

    bool m_is_dir { false };

private:
    std::shared_ptr<node> m_mounted_node {};
};

size_t new_descriptor(node &node);

struct vfs_root : public node
{
    vfs_root() : node(nullptr) {m_is_dir = true;}

    virtual std::string name() const override { return ""; }
    virtual bool is_dir() const override { return true; }

private:
    std::shared_ptr<node> add_node(const std::string& name, bool dir);

protected:
    virtual std::shared_ptr<node> mkdir_impl(const std::string& str) override { return add_node(str, true); }
    virtual std::shared_ptr<node> touch_impl(const std::string& str) override { return add_node(str, false); }
    virtual std::vector<std::shared_ptr<node>> readdir_impl() override { return m_children; }
    virtual bool remove_impl(const vfs::node* child) override;

private:
    std::vector<std::shared_ptr<node>> m_children;
};

struct symlink : public node
{

    symlink(node& target)
        : m_target(target)
    {

    }

    virtual size_t size() const override { return m_target.size(); }
    virtual bool is_dir() const override { return m_target.is_dir(); }
    virtual bool is_link() const override { return true; }

    node& target() const { return m_target; }

protected:

    [[nodiscard]] virtual MemBuffer read_impl(size_t offset, size_t size) const override { return m_target.read(offset, size); }
    [[nodiscard]] virtual bool write_impl(size_t offset, gsl::span<const uint8_t> data) override { return m_target.write(offset, data); }
    virtual std::vector<std::shared_ptr<node>> readdir_impl() override { return m_target.readdir_impl(); }
    [[nodiscard]] virtual std::shared_ptr<node> mkdir_impl(const std::string& s) override { return m_target.mkdir(s); };
    [[nodiscard]] virtual std::shared_ptr<node> touch_impl(const std::string& s) override { return m_target.touch(s); }

private:
    node& m_target;
};

void init();

std::shared_ptr<node> find(const std::string& path);

bool mount(std::shared_ptr<node> target, std::shared_ptr<node> mountpoint);
bool umount(std::shared_ptr<node> target);

void traverse(const vfs::node& node, size_t indent = 0);
void traverse(const std::string& path);

bool is_symlink(const vfs::node& node);
node& link_target(const vfs::node& link);

extern std::vector<std::reference_wrapper<node>> descriptors;
extern std::shared_ptr<vfs_root> root;
extern std::vector<node*> mounted_nodes;
};

#endif // VFS_HPP
