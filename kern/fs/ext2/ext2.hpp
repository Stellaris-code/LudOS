/*
ext2.hpp

Copyright (c) 19 Yann BOUCHER (yann)

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
#ifndef EXT2_HPP
#define EXT2_HPP

#include "fs/fs.hpp"

#include <optional.hpp>

#include "ext2_structures.hpp"

class Ext2FS : public FSImpl<Ext2FS>
{
    friend class ext2_node;

public:
    Ext2FS(Disk& disk);

    static bool accept(const Disk& disk);

    virtual std::shared_ptr<vfs::node> root() const override;

    virtual std::string type() const override { return "ext2"; }
    virtual size_t total_size() const override { return m_superblock.block_count*block_size(); }
    virtual size_t free_size() const override { return m_superblock.unallocated_blocks*block_size(); }

    virtual void umount() override;

private:
    static std::optional<const ext2::Superblock> read_superblock(const Disk& disk);

    static bool check_superblock(const ext2::Superblock&);

    bool check_superblock_backups() const;

    size_t block_group_table_block() const;
    const ext2::BlockGroupDescriptor get_block_group(size_t inode) const;
    MemBuffer read_block(size_t number) const;
    void write_block(size_t number, gsl::span<const uint8_t> data);
    const ext2::Inode read_inode(size_t inode) const;
    std::vector<const ext2::DirectoryEntry> read_directory_entries(size_t inode) const;
    bool check_inode_presence(size_t inode) const;

    MemBuffer read_data(const ext2::Inode& inode, size_t offset, size_t size) const;
    MemBuffer read_data_block(const ext2::Inode& inode, size_t blk_id) const;
    MemBuffer read_indirected(size_t indirected_block, size_t blk_id, size_t depth) const;

    void write_data(gsl::span<const uint8_t> data, const ext2::Inode &inode, size_t offset, size_t size);
    void write_data_block(gsl::span<const uint8_t> data, const ext2::Inode &inode, size_t blk_id);
    void write_indirected(gsl::span<const uint8_t> data, size_t indirected_block, size_t blk_id, size_t depth);

    void update_superblock();

    void resize_inode(size_t inode, size_t size);
    std::vector<const ext2::DirectoryEntry> read_directory(gsl::span<const uint8_t> data) const;
    void write_directory_entries(size_t inode, gsl::span<const ext2::DirectoryEntry> entries);

    uint16_t inode_size() const;
    uint32_t block_size() const;

    void error(const std::string& message) const;

private:
    ext2::Superblock m_superblock;
};

class ext2_node : public vfs::node
{
public:
    ext2_node(Ext2FS& p_fs, vfs::node* p_parent, const std::string& p_name, size_t p_inode)
        : vfs::node(p_parent), fs(p_fs), inode(p_inode), filename(p_name), inode_struct(fs.read_inode(inode))
    {
    }

    virtual ~ext2_node()
    {
        fs.umount();
    }

    virtual std::string name() const override;
    virtual void rename_impl(const std::string& s) override;
    virtual Stat stat() const override;
    virtual void set_stat(const Stat& stat) override;

    [[nodiscard]] virtual MemBuffer read_impl(size_t offset, size_t size) const override;
    [[nodiscard]] virtual bool write_impl(size_t offset, gsl::span<const uint8_t> data) override { return false; }
    virtual std::vector<std::shared_ptr<node>> readdir_impl() override;
    [[nodiscard]] virtual std::shared_ptr<node> mkdir_impl(const std::string&) override { return nullptr; }
    [[nodiscard]] virtual std::shared_ptr<node> touch_impl(const std::string&) override { return nullptr; }
    virtual size_t size() const override { return inode_struct.size_lower; }
    virtual bool is_dir() const override { return inode_struct.type & (int)ext2::InodeType::Directory; }

public:

    Ext2FS& fs;
    const size_t inode;
    const ext2::Inode inode_struct;
    std::string filename;

private:
    void update_dir_entry(size_t inode, const std::string& name, uint8_t type);
};

#endif // EXT2_HPP
