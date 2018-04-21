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
#include <utils/gsl/gsl_span.hpp>

#include "utils/membuffer.hpp"

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

    enum Type
    {
        Unknown = 0,
        File = 1,
        Directory = 2,
        SymLink = 3
    };

    struct Stat
    {
        uint16_t perms { OtherRead };
        uint16_t uid { 0 };
        uint16_t gid { 0 };
        uint32_t flags { 0 };
        size_t access_time { 0 };
        size_t creation_time { 0 };
        size_t modification_time { 0 };
    };

    node(node* parent = nullptr);

    node(const node&) = delete;
    node(node&&) = default;

    virtual ~node();

    virtual std::string name() const { return m_name; }
    void rename(const std::string& name);

    virtual Stat stat() const { return m_stat; }
    virtual void set_stat(const Stat& stat) { m_stat = stat; }

    virtual size_t size() const { return 0; }
    virtual Type type() const { return m_type; }
    virtual bool is_link() const { return false; }

    [[nodiscard]] MemBuffer read(size_t offset, size_t size) const;
    [[nodiscard]] MemBuffer read() const { return read(0, size()); }
    [[nodiscard]] bool write(size_t offset, gsl::span<const uint8_t> data);
    [[nodiscard]] std::shared_ptr<node> create(const std::string&, Type);
    bool resize(size_t);
    std::vector<std::shared_ptr<node>> readdir();
    std::vector<std::shared_ptr<const node>> readdir() const;
    bool remove(const vfs::node* child);

    node* parent() const { return m_parent; }
    void set_parent(node* parent) { m_parent = parent; }

    std::string path() const;

protected:
    [[nodiscard]] virtual MemBuffer read_impl(size_t, size_t) const { return {}; }
    [[nodiscard]] virtual bool write_impl(size_t, gsl::span<const uint8_t>) { return false; }
    virtual bool resize_impl(size_t) { return false; }
    virtual std::vector<std::shared_ptr<node>> readdir_impl() { return {}; }
    [[nodiscard]] virtual std::shared_ptr<node> create_impl(const std::string&, Type) { return nullptr; }
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

    Type m_type { File };

private:
    std::shared_ptr<node> m_mounted_node {};
};

struct vfs_root : public node
{
    vfs_root() : node(nullptr) { m_type = Directory; }

    virtual std::string name() const override { return ""; }
    virtual Type type() const override { return Directory; }

private:
    std::shared_ptr<node> add_node(const std::string& name, Type type);

protected:
    virtual std::shared_ptr<node> create_impl(const std::string& str, Type type) override
    { return add_node(str, type); }
    virtual std::vector<std::shared_ptr<node>> readdir_impl() override { return m_children; }
    virtual bool remove_impl(const vfs::node* child) override;

private:
    std::vector<std::shared_ptr<node>> m_children;
};

struct symlink : public node
{

    symlink(std::string target);

    symlink(std::string target, std::string name);

    virtual size_t size() const override { return actual_target()->size(); }
    virtual Type type() const override { return actual_target()->type(); }
    virtual bool is_link() const override { return true; }
    virtual std::string name() const override;

    std::string target() const { return m_target; }

private:
    std::shared_ptr<node> actual_target();
    std::shared_ptr<const node> actual_target() const;

protected:
    [[nodiscard]] virtual MemBuffer read_impl(size_t offset, size_t size) const override { return actual_target()->read(offset, size); }
    [[nodiscard]] virtual bool write_impl(size_t offset, gsl::span<const uint8_t> data) override { return actual_target()->write(offset, data); }
    virtual std::vector<std::shared_ptr<node>> readdir_impl() override { return actual_target()->readdir_impl(); }
    [[nodiscard]] virtual std::shared_ptr<node> create_impl(const std::string& s, Type type) override { return actual_target()->create(s, type); };

private:
    std::string m_target;
    std::string m_linkname;
};

extern std::shared_ptr<vfs_root> root;
extern std::vector<std::weak_ptr<node>> mounted_nodes;

void init();
};

#endif // VFS_HPP
