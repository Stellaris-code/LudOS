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

public:
    static std::optional<const ext2::Superblock> read_superblock(const Disk& disk);

    static bool check_superblock(const ext2::Superblock&);

    bool check_superblock_backups() const;

    size_t block_group_table_block() const;
    const ext2::BlockGroupDescriptor get_block_group(size_t idx) const;
    void write_block_group(size_t idx, const ext2::BlockGroupDescriptor& desc);
    MemBuffer read_block(size_t number) const;
    void write_block(size_t number, gsl::span<const uint8_t> data);
    const ext2::Inode read_inode(size_t inode) const;
    void write_inode(size_t inode, const ext2::Inode& structure);
    bool check_inode_presence(size_t inode) const;

    size_t data_blocks(const ext2::Inode& inode) const;
    size_t get_data_block(const ext2::Inode& inode, size_t blk_id) const;
    size_t get_data_block_indirected(size_t indirected_block, size_t blk_id, size_t depth) const;
    size_t count_used_blocks(const ext2::Inode& inode) const;

    MemBuffer read_data(const ext2::Inode& inode, size_t offset, size_t size) const;
    MemBuffer read_data_block(const ext2::Inode& inode, size_t blk_id) const;
    MemBuffer read_indirected(size_t indirected_block, size_t blk_id, size_t depth) const;

    void alloc_data_block(size_t inode, size_t blk_id);
    void alloc_indirected(size_t indirected_block, size_t blk_id, size_t block_group, size_t depth);

    void free_data_block(size_t inode, size_t block);
    void free_indirected(size_t indirected_block, size_t blk_id, size_t depth);

    size_t alloc_block(size_t preferred_group);
    size_t alloc_block_in_block_group(size_t group);
    void   free_block(size_t block);
    size_t alloc_inode(size_t preferred_group, bool directory);
    size_t alloc_inode_in_block_group(size_t group, bool directory);

    void write_offsetted_data(gsl::span<const uint8_t> data, size_t block, size_t byte_off, const ext2::Inode &inode);
    void write_data(gsl::span<const uint8_t> data, size_t byte_offset, const ext2::Inode &inode);
    void write_data_block(gsl::span<const uint8_t> data, const ext2::Inode &inode, size_t blk_id);
    void write_indirected(gsl::span<const uint8_t> data, size_t indirected_block, size_t blk_id, size_t depth);

    void update_superblock();

    void resize_inode(size_t inode, size_t size);
    void remove_inode(size_t inode, bool dir);
    void decrease_link_count(size_t inode);

    std::string link_name(const ext2::Inode& inode_struct);

    std::vector<ext2::DirectoryEntry> read_directory(gsl::span<const uint8_t> data) const;

    ext2::DirectoryEntry create_dir_entry(size_t inode, uint8_t type, const std::string& name);

    std::vector<ext2::DirectoryEntry> read_directory_entries(size_t inode) const;
    void write_directory_entries(size_t inode, gsl::span<ext2::DirectoryEntry> entries);

    uint16_t inode_size() const;
    uint32_t block_size() const;

    void error(const std::string& message) const;

private:
    ext2::Superblock m_superblock;
    mutable uint16_t m_has_error { true };
};

class ext2_node : public vfs::node
{
public:
    ext2_node(Ext2FS& p_fs, vfs::node* p_parent, const std::string& p_name, size_t p_inode)
        : vfs::node(p_parent), fs(p_fs), inode(p_inode), filename(p_name)
    {
    }

    virtual ~ext2_node()
    {
        if (inode == (uint16_t)ext2::ReservedInodes::RootInode) fs.umount(); // root
    }

    virtual std::string name() const override;
    virtual void rename_impl(const std::string& s) override;

    virtual Stat stat() const override;
    virtual void set_stat(const Stat& stat) override;

    [[nodiscard]] virtual MemBuffer read_impl(size_t offset, size_t size) const override;
    [[nodiscard]] virtual bool write_impl(size_t offset, gsl::span<const uint8_t> data) override;
    virtual std::vector<std::shared_ptr<node>> readdir_impl() override;
    [[nodiscard]] virtual std::shared_ptr<node> create_impl(const std::string&, Type type) override;
    virtual bool resize_impl(size_t size) override;
    virtual bool remove_impl(const vfs::node*) override;
    virtual size_t size() const override;
    virtual Type type() const override;
    virtual bool is_link() const override;

public:
    Ext2FS& fs;
    const size_t inode;
    std::string filename;

private:
    void remove_child(const std::string &name);
    ext2::InodeType vfs_type_to_ext2_inode(Type type);
    ext2::DirectoryType vfs_type_to_ext2_dir(Type type);
    void update_dir_entry(size_t inode, const std::string& name);
    void write_inode_struct(const ext2::Inode &inode_struct);
    std::shared_ptr<ext2_node> create_child(const std::string& name, Type type);
    std::string link_name() const;
    std::shared_ptr<vfs::node> link_target() const;
};

#endif // EXT2_HPP
