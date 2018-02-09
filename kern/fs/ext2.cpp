/*
ext2.cpp

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

#include "ext2.hpp"

#include "utils/memutils.hpp"
#include "time/time.hpp"
#include "utils/bitops.hpp"
#include "utils/vecutils.hpp"

Ext2FS::Ext2FS(Disk &disk) : FSImpl<Ext2FS>(disk)
{
    m_superblock = *read_superblock(disk);
}

bool Ext2FS::accept(const Disk &disk)
{
    auto superblock = read_superblock(disk);
    if (!superblock) return false;

    return superblock->ext2_signature == ext2::signature;
}

std::shared_ptr<vfs::node> Ext2FS::root() const
{
    return std::make_shared<ext2_node>(*this, nullptr, 2);
}

std::optional<const ext2::Superblock> Ext2FS::read_superblock(const Disk &disk)
{
    if (disk.disk_size() < 1024*2)
    {
        return {};
    }

    auto data = disk.read(1024, 1024);
    return *reinterpret_cast<const ext2::Superblock*>(data.data());
}

size_t Ext2FS::block_group_table_block() const
{
    return m_superblock.block_size == 0 ? 2 : 1;
}

const ext2::BlockGroupDescriptor Ext2FS::get_block_group(size_t inode) const
{
    size_t offset = (inode - 1) / m_superblock.inodes_in_block_group;
    size_t block = block_group_table_block() + (offset / (block_size()/sizeof(ext2::BlockGroupDescriptor)));
    auto data = read_block(block);
    return ((ext2::BlockGroupDescriptor*)data.data())[offset];
}

std::vector<uint8_t> Ext2FS::read_block(size_t number) const
{
    assert(number < m_superblock.block_count);
    auto vec = m_disk.read(number * block_size(), block_size());
    return vec;
}

const ext2::Inode Ext2FS::read_inode(size_t inode) const
{
    if (!check_inode_presence(inode))
    {
        warn("Inode %d is marked as free\n", inode);
    }

    auto block_group = get_block_group(inode);
    size_t index = (inode - 1) % m_superblock.inodes_in_block_group;
    size_t block_idx = (index * inode_size()) / block_size();
    size_t offset = index % (block_size() / inode_size());

    auto block = read_block(block_group.inode_table + block_idx);
    return *((ext2::Inode*)block.data() + offset);
}

bool Ext2FS::check_inode_presence(size_t inode) const
{
    auto block_group = get_block_group(inode);
    size_t index = (inode - 1) % m_superblock.inodes_in_block_group;
    size_t block_idx = index / block_size();
    size_t offset = index % block_size();

    auto block = read_block(block_group.inode_bitmap + block_idx);

    return bit_check(block[offset / 8], offset % 8);
}

std::vector<uint8_t> Ext2FS::read_data_block(const ext2::Inode &inode, size_t blk_id) const
{
    size_t entries_per_block = block_size()/sizeof(uint32_t);

    if (blk_id < 12)
    {
        return read_block(inode.block_ptr[blk_id]);
    }
    blk_id -= 12;

    if (blk_id < entries_per_block)
    {
        return read_singly_indirected(inode.block_ptr[12], blk_id);
    }
    blk_id -= entries_per_block;

    if (blk_id < entries_per_block*entries_per_block)
    {
        return read_doubly_indirected(inode.block_ptr[13], blk_id);
    }
    blk_id -= entries_per_block*entries_per_block;

    return read_triply_indirected(inode.block_ptr[14], blk_id);
}

std::vector<uint8_t> Ext2FS::read_data(const ext2::Inode &inode) const
{
#if 0
    std::vector<uint8_t> data;
    size_t blocks = inode.blocks_512 / (block_size()/512) + (inode.blocks_512%(block_size()/512)?1:0);
    for (size_t i { 0 }; i < 12; ++i)
    {
        if (blocks == 0 || inode.block_ptr[i] == 0) break;
        merge(data, read_block(inode.block_ptr[i]));
        --blocks;
    }

    merge(data, read_singly_indirected_old(inode.block_ptr[12], blocks));
    merge(data, read_doubly_indirected_old(inode.block_ptr[13], blocks));
    merge(data, read_triply_indirected(inode.block_ptr[14], blocks));

    return data;
#else
    size_t blocks = inode.blocks_512 / (block_size()/512) + (inode.blocks_512%(block_size()/512)?1:0);
    std::vector<uint8_t> data;
    data.reserve(blocks);

    for (size_t i { 0 }; i < blocks; ++i)
    {
        merge(data, read_data_block(inode, i));
    }

    return data;
#endif
}

std::vector<uint8_t> Ext2FS::read_singly_indirected(size_t indirected_block, size_t blk_id) const
{
    std::vector<uint8_t> data;
    assert(blk_id < block_size()/sizeof(uint32_t));
    auto vec = read_block(indirected_block);
    uint32_t* block = (uint32_t*)vec.data();

    return read_block(block[blk_id]);
}
std::vector<uint8_t> Ext2FS::read_doubly_indirected(size_t indirected_block, size_t blk_id) const
{
    std::vector<uint8_t> data;
    size_t id_num = block_size()/sizeof(uint32_t);
    auto vec = read_block(indirected_block);
    uint32_t* block = (uint32_t*)vec.data();

    size_t singly_block_idx = blk_id / id_num;
    size_t offset = blk_id % id_num;

    return read_singly_indirected(block[singly_block_idx], offset);
}

std::vector<uint8_t> Ext2FS::read_triply_indirected(size_t indirected_block, size_t blk_id) const
{
    std::vector<uint8_t> data;
    size_t id_num = block_size()/sizeof(uint32_t);
    auto vec = read_block(indirected_block);
    uint32_t* block = (uint32_t*)vec.data();

    size_t doubly_block_idx = blk_id / (id_num*id_num);
    size_t offset = blk_id % (id_num*id_num);

    return read_doubly_indirected(block[doubly_block_idx], offset);
}

std::vector<uint8_t> Ext2FS::read_singly_indirected_old(size_t block_id, size_t& blocks) const
{
    std::vector<uint8_t> data;

    for (size_t i { 0 }; i < block_size()/sizeof(uint32_t); ++i)
    {
        if (blocks == 0) return data;
        merge(data, read_singly_indirected(block_id, i));
        --blocks;
    }

    return data;
}

std::vector<uint8_t> Ext2FS::read_doubly_indirected_old(size_t block_id, size_t &blocks) const
{
    size_t id_num = block_size()/sizeof(uint32_t);
    std::vector<uint8_t> data;

    for (size_t i { 0 }; i < id_num*id_num; ++i)
    {
        if (blocks == 0) return data;
        merge(data, read_doubly_indirected(block_id, i));
        --blocks;
    }

    return data;
}

std::vector<uint8_t> Ext2FS::read_triply_indirected_old(size_t block_id, size_t &blocks) const
{
#if 0
    std::vector<uint8_t> data;
    size_t id_num = block_size()/sizeof(uint32_t);
    auto vec = read_block(block_id);
    uint32_t* block = (uint32_t*)vec.data();

    for (size_t i { 0 }; i < id_num; ++i)
    {
        if (blocks == 0 || block[i] == 0) return data;
        merge(data, read_doubly_indirected(block[i], blocks));
    }

    return data;
#else
    size_t id_num = block_size()/sizeof(uint32_t);
    std::vector<uint8_t> data;

    for (size_t i { 0 }; i < id_num*id_num*id_num; ++i)
    {
        if (blocks == 0) return data;
        merge(data, read_triply_indirected(block_id, i));
        --blocks;
    }

    return data;
#endif
}

std::vector<const ext2::DirectoryEntry> Ext2FS::read_directory(const std::vector<uint8_t> &data) const
{
    std::vector<const ext2::DirectoryEntry> entries;
    const uint8_t* ptr = (const uint8_t*)data.data();

    while (ptr <= &data.back())
    {
        if (((ext2::DirectoryEntry*)ptr)->inode) entries.emplace_back(*(const ext2::DirectoryEntry*)ptr);
        ptr += entries.back().record_len;
    }

    return entries;
}

uint16_t Ext2FS::inode_size() const
{
    return m_superblock.version_major>=1?m_superblock.inode_size:128;
}

uint32_t Ext2FS::block_size() const
{
    return 1024<<m_superblock.block_size;
}

void ext2_node::rename(const std::string &s)
{
    m_name = s;
    if (parent())
    {
        static_cast<ext2_node*>(parent())->update_dir_entry(m_inode, s,
                                                            (uint8_t)(is_dir() ? ext2::DirectoryType::Directory : ext2::DirectoryType::Regular));
    }
}

std::vector<uint8_t> ext2_node::read(size_t offset, size_t size) const
{
    if (is_dir()) return {};

    auto data = m_fs.read_data(m_inode_struct);
    return std::vector<uint8_t>(data.begin() + offset, data.begin() + offset + size);
}

std::vector<std::shared_ptr<vfs::node>> ext2_node::readdir_impl()
{
    if (!is_dir()) return {};

    std::vector<std::shared_ptr<vfs::node>> vec;

    for (const auto& entry : m_fs.read_directory(m_fs.read_data(m_inode_struct)))
    {
        if (std::string(entry.name, entry.name_len) != "." &&
                std::string(entry.name, entry.name_len) != "..")
        {
            vec.push_back(std::static_pointer_cast<vfs::node>(
                              std::make_shared<ext2_node>(m_fs, this, entry.inode)));
            vec.back()->m_name = std::string(entry.name, entry.name_len);
        }
    }

    return vec;
}

void ext2_node::update_dir_entry(size_t inode, const std::string &name, uint8_t type)
{
    // TODO
}

ADD_FS(Ext2FS)
