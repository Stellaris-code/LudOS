/*
ext2_write.cpp

Copyright (c) 18 Yann BOUCHER (yann)

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

#include "time/time.hpp"
#include "utils/bitops.hpp"
#include "utils/vecutils.hpp"
#include "utils/mathutils.hpp"
#include "utils/memutils.hpp"
#include "utils/stlutils.hpp"

#include <string.h>

void Ext2FS::umount()
{
    m_superblock.fs_state = m_has_error;

    update_superblock();
}

void Ext2FS::update_superblock()
{
    m_superblock.last_write_time = Time::epoch();

    const size_t last_group = m_superblock.block_count / m_superblock.blocks_in_block_group;

    m_disk.write(1024, {(uint8_t*)&m_superblock, sizeof(ext2::Superblock)});

    for (size_t i { 1 }; i <= last_group; ++i)
    {
        bool check = false;
        if (m_superblock.version_major==0) check = true; // revision 0 puts a backup at every group
        if (m_superblock.version_major>=1)
        {
            if (i == 1) check = true;
            if (is_power_of(i, 3ul) || is_power_of(i, 5ul) || is_power_of(i, 7ul)) check = true;
        }

        if (check)
        {
            write_block(i * m_superblock.blocks_in_block_group + 1, {(uint8_t*)&m_superblock, sizeof(ext2::Superblock)});
        }
    }
}

void Ext2FS::write_block(size_t number, gsl::span<const uint8_t> data)
{
    assert(number < m_superblock.block_count);

    m_disk.write(number * block_size(), data);
}

void Ext2FS::write_offsetted_data(gsl::span<const uint8_t> data, size_t block, size_t byte_off, const ext2::Inode &inode)
{
    assert((size_t)data.size() <= block_size());

    gsl::span<const uint8_t> spans[2];
    spans[0] = data.subspan(0, std::min<size_t>(block_size() - byte_off, data.size()));

    if (block_size() - byte_off < (size_t)data.size())
    {
        spans[1] = data.subspan(block_size() - byte_off);
    }

    for (size_t i { 0 }; i < 2; ++i)
    {
        auto sect_data = read_data_block(inode, block+i);

        assert((size_t)spans[i].size() <= sect_data.size() - byte_off);

        std::copy(spans[i].begin(), spans[i].end(), sect_data.begin() + byte_off);
        write_data_block(sect_data, inode, block+i);

        byte_off = 0;
    }
}

void Ext2FS::write_data(gsl::span<const uint8_t> data, size_t byte_offset, const ext2::Inode &inode)
{
    const size_t blocks = data_blocks(inode);
    const size_t block_offset = byte_offset/block_size();
    const size_t count = data.size()/block_size() + (data.size()%block_size()?1:0);

    assert(block_offset + count <= blocks);

    auto chunks = split(data, block_size());

    for (size_t i { 0 }; i < count; ++i)
    {
        write_offsetted_data(chunks[i], block_offset + i, byte_offset % block_size(), inode);
        byte_offset -= block_size();
    }
}

void Ext2FS::write_data_block(gsl::span<const uint8_t> data, const ext2::Inode &inode, size_t blk_id)
{
    assert(data.size() <= (int)block_size());

    size_t entries_per_block = block_size()/sizeof(uint32_t);

    if (blk_id < 12)
    {
        write_block(inode.block_ptr[blk_id], data);
        return;
    }
    blk_id -= 12;

    if (blk_id < entries_per_block)
    {
        write_indirected(data, inode.block_ptr[12], blk_id, 1);
        return;
    }
    blk_id -= entries_per_block;

    if (blk_id < entries_per_block*entries_per_block)
    {
        write_indirected(data, inode.block_ptr[13], blk_id, 2);
        return;
    }
    blk_id -= entries_per_block*entries_per_block;

    write_indirected(data, inode.block_ptr[14], blk_id, 3);
    return;
}

void Ext2FS::write_indirected(gsl::span<const uint8_t> data, size_t indirected_block, size_t blk_id, size_t depth)
{
    assert(data.size() <= (int)block_size());

    size_t entries = ipow<size_t>(block_size()/sizeof(uint32_t), depth-1);
    auto vec = read_block(indirected_block);
    uint32_t* block = (uint32_t*)vec.data();

    if (depth <= 1) write_block(block[blk_id], data);
    else
    {
        size_t tgt_block_idx = blk_id / entries;
        size_t offset = blk_id % entries;

        write_indirected(data, block[tgt_block_idx], offset, depth - 1);
    }
}

// TODO : rewrite les autres avec callback
void Ext2FS::resize_inode(size_t inode, size_t size)
{
    auto info = read_inode(inode);

    size_t org_blocks = data_blocks(info);
    size_t tgt_blocks = size / block_size() + (size%block_size()?1:0);

    // allocate more blocks
    if (tgt_blocks > org_blocks)
    {
        for (size_t i { 0 }; i < tgt_blocks - org_blocks; ++i)
        {
            alloc_data_block(inode, org_blocks + i);
        }
    }
    else if (tgt_blocks < org_blocks)
    {
        for (size_t i { org_blocks }; i > tgt_blocks; --i)
        {
            free_data_block(inode, i - 1);
        }
    }

    info = read_inode(inode);

    info.size_lower = size;
    write_inode(inode, info);

    const size_t used_blocks = count_used_blocks(info) * block_size();
    info.blocks_512 = used_blocks / 512 + (used_blocks%512?1:0);

    write_inode(inode, info);
}

void Ext2FS::remove_inode(size_t inode, bool dir)
{
    auto bgd = get_block_group((inode - 1) / m_superblock.inodes_in_block_group);

    ++bgd.free_inodes_count;
    if (dir) --bgd.used_dirs_count;

    auto bitmap = read_block(bgd.inode_bitmap);
    size_t index = (inode - 1) % m_superblock.inodes_in_block_group;

    bit_clear(bitmap[index / 8], index % 8);
    write_block(bgd.inode_bitmap, bitmap);
    write_block_group((inode - 1) / m_superblock.inodes_in_block_group, bgd);

    ++m_superblock.unallocated_inodes;
    update_superblock();
}

void Ext2FS::decrease_link_count(size_t inode)
{
    auto info = read_inode(inode);
    if (info.links_count) --info.links_count;

    // erase the inode
    if (info.links_count == 0)
    {
        info.deletion_time = Time::epoch();
        write_inode(inode, info);

        resize_inode(inode, 0); // deallocate blocks
        remove_inode(inode, (info.type & 0xF000) == (uint16_t)ext2::InodeType::Directory);
    }
    else
    {
        write_inode(inode, info);
    }
}

void Ext2FS::write_directory_entries(size_t inode, gsl::span<ext2::DirectoryEntry> entries)
{
    std::vector<ext2::DirectoryEntry> copied_entries(entries.size());
    std::copy(entries.begin(), entries.end(), copied_entries.begin());

    size_t cursor = 0;
    for (size_t i { 0 }; i < (size_t)copied_entries.size(); ++i)
    {
        auto& entry = copied_entries[i];

        entry.record_len = 8 + entry.name_len;

        const bool same_block = cursor/block_size() == (cursor + entry.record_len)/block_size();

        if (!same_block)
        {
            size_t offset = ((cursor + entry.record_len)/block_size())*block_size() - cursor;

            if ((cursor + offset) % 4) offset += 4 - (cursor + offset)%4; // align to 4 byte

            copied_entries[i-1].record_len += offset;
            assert((cursor+offset)/block_size() == cursor/block_size()+1);
            cursor += offset;
        }

        if ((cursor + entry.record_len) % 4)  // align to 4 byte
        {
            entry.record_len += 4 - (cursor + entry.record_len)%4;
        }

        cursor += entry.record_len;

        if (i == copied_entries.size()-1 && cursor%block_size()) // last entry
        {
            const size_t distance = block_size() - cursor%block_size();
            entry.record_len += distance;
            cursor += distance;
        }
    }

    assert(cursor % block_size() == 0);

    resize_inode(inode, cursor);

    MemBuffer data(cursor, 0);

    cursor = 0;
    for (const auto& entry : copied_entries)
    {
        assert(entry.record_len >= 8 + entry.name_len);
        assert(entry.inode);

        std::copy((uint8_t*)&entry, (uint8_t*)&entry + 8 + entry.name_len, data.begin() + cursor);

        cursor += entry.record_len;

        assert(cursor % 4 == 0);
    }
    assert(cursor == data.size());

    write_data(data, 0, read_inode(inode));
}

void Ext2FS::write_inode(size_t inode, const ext2::Inode& structure)
{
    if (!check_inode_presence(inode))
    {
        error(("Inode " + kpp::to_string(inode) + " is marked as free\n").c_str());
        return;
    }

    auto block_group = get_block_group((inode - 1) / m_superblock.inodes_in_block_group);

    size_t index = (inode - 1) % m_superblock.inodes_in_block_group;
    size_t block_idx = (index * inode_size()) / block_size();
    size_t offset = index % (block_size() / inode_size());

    auto block = read_block(block_group.inode_table + block_idx);

    std::copy((const uint8_t*)&structure,
              (const uint8_t*)&structure + inode_size(),
              block.data() + offset*inode_size());

    write_block(block_group.inode_table + block_idx, block);
}

void Ext2FS::write_block_group(size_t idx, const ext2::BlockGroupDescriptor &desc)
{
    const size_t group_desc_per_block = block_size()/sizeof(ext2::BlockGroupDescriptor);

    const size_t block_to_write = block_group_table_block() + (idx / group_desc_per_block);

    auto data = read_block(block_to_write);

    std::copy((const uint8_t*)&desc, (const uint8_t*)&desc + sizeof(desc),
              data.data() + (idx % group_desc_per_block) * sizeof(desc));

    write_block(block_to_write, data);
}

size_t Ext2FS::alloc_block(size_t preferred_group)
{
    size_t block_groups = m_superblock.block_count / m_superblock.blocks_in_block_group +
            (m_superblock.block_count%m_superblock.blocks_in_block_group?1:0);

    size_t block = alloc_block_in_block_group(preferred_group);
    if (block) goto allocate;

    for (size_t i { 0 }; i < block_groups; ++i)
    {
        if (i != preferred_group)
        {
            block = alloc_block_in_block_group(i);
            if (block) goto allocate;
        }
    }

    return 0;

allocate:
    static std::vector<uint8_t> zeroes(block_size(), 0);
    write_block(block, zeroes); // zero out the block

    --m_superblock.unallocated_blocks;
    update_superblock();

    return block;
}

size_t Ext2FS::alloc_block_in_block_group(size_t group)
{
    auto bgd = get_block_group(group);
    if (bgd.free_blocks_count == 0) return 0;

    auto bitmap = read_block(bgd.block_bitmap);
    for (size_t j { 0 }; j < block_size()*8; ++j)
    {
        if (!bit_check(bitmap[j / 8], j % 8))
        {
            --bgd.free_blocks_count;
            bit_set(bitmap[j / 8], j % 8);
            write_block(bgd.block_bitmap, bitmap);

            write_block_group(group, bgd);

            return j + group*m_superblock.blocks_in_block_group + 1;
        }
    }

    return 0;
}

void Ext2FS::alloc_data_block(size_t inode, size_t blk_id)
{
    size_t entries_per_block = block_size()/sizeof(uint32_t);

    auto info = read_inode(inode);

    const size_t block_group = inode / m_superblock.inodes_in_block_group;

    if (blk_id < 12)
    {
        info.block_ptr[blk_id] = alloc_block(block_group);
        write_inode(inode, info);
        return;
    }
    blk_id -= 12;

    if (blk_id < entries_per_block)
    {
        if (!info.block_ptr[12])
        {
            info.block_ptr[12] = alloc_block(block_group);
            write_inode(inode, info);
        }
        alloc_indirected(info.block_ptr[12], blk_id, block_group, 1);
        return;
    }
    blk_id -= entries_per_block;

    if (blk_id < entries_per_block*entries_per_block)
    {
        if (!info.block_ptr[13])
        {
            info.block_ptr[13] = alloc_block(block_group);
            write_inode(inode, info);
        }
        alloc_indirected(info.block_ptr[13], blk_id, block_group, 2);
        return;
    }
    blk_id -= entries_per_block*entries_per_block;

    if (!info.block_ptr[14])
    {
        info.block_ptr[14] = alloc_block(block_group);
        write_inode(inode, info);
    }
    alloc_indirected(info.block_ptr[14], blk_id, block_group, 3);
    return;
}

void Ext2FS::alloc_indirected(size_t indirected_block, size_t blk_id, size_t block_group, size_t depth)
{
    MemBuffer data;
    size_t entries = ipow<size_t>(block_size()/sizeof(uint32_t), depth-1);
    auto vec = read_block(indirected_block);
    uint32_t* block = (uint32_t*)vec.data();

    if (depth <= 1)
    {
        block[blk_id] = alloc_block(block_group);
        write_block(indirected_block, vec);
    }
    else
    {
        size_t tgt_block_idx = blk_id / entries;
        size_t offset = blk_id % entries;

        if (!block[tgt_block_idx])
        {
            block[tgt_block_idx] = alloc_block(block_group);
            write_block(indirected_block, vec);
        }
        alloc_indirected(block[tgt_block_idx], offset, block_group, depth - 1);
    }
}

void Ext2FS::free_block(size_t block)
{
    auto bgd = get_block_group((block - 1) / m_superblock.blocks_in_block_group);

    ++bgd.free_blocks_count;

    auto bitmap = read_block(bgd.block_bitmap);
    size_t index = (block - 1) % m_superblock.blocks_in_block_group;

    assert(index / 8 < bitmap.size());

    if (!bit_check(bitmap[index / 8], index % 8))
    {
        error(("Double free, block " + kpp::to_string(block) + "\n").c_str());
    }

    bit_clear(bitmap[index / 8], index % 8);
    write_block(bgd.block_bitmap, bitmap);
    write_block_group((block - 1) / m_superblock.blocks_in_block_group, bgd);

    ++m_superblock.unallocated_blocks;
    update_superblock();
}

void Ext2FS::free_data_block(size_t inode, size_t blk_id)
{
    size_t entries_per_block = block_size()/sizeof(uint32_t);

    auto info = read_inode(inode);

    if (blk_id < 12)
    {
        free_block(info.block_ptr[blk_id]);
        info.block_ptr[blk_id] = 0;
        write_inode(inode, info);
        return;
    }
    blk_id -= 12;

    if (blk_id < entries_per_block)
    {
        free_indirected(info.block_ptr[12], blk_id, 1);
        if (blk_id == 0)
        {
            free_block(info.block_ptr[12]);
            info.block_ptr[12] = 0;
            write_inode(inode, info);
        }
        return;
    }
    blk_id -= entries_per_block;

    if (blk_id < entries_per_block*entries_per_block)
    {
        free_indirected(info.block_ptr[13], blk_id, 2);
        if (blk_id == 0)
        {
            free_block(info.block_ptr[13]);
            info.block_ptr[13] = 0;
            write_inode(inode, info);
        }
        return;
    }
    blk_id -= entries_per_block*entries_per_block;

    free_indirected(info.block_ptr[14], blk_id, 3);
    if (blk_id == 0)
    {
        free_block(info.block_ptr[14]);
        info.block_ptr[14] = 0;
        write_inode(inode, info);
    }
    return;
}

void Ext2FS::free_indirected(size_t indirected_block, size_t blk_id, size_t depth)
{
    assert(indirected_block);

    size_t entries = ipow<size_t>(block_size()/sizeof(uint32_t), depth-1);
    auto vec = read_block(indirected_block);
    uint32_t* block = (uint32_t*)vec.data();

    if (depth <= 1)
    {
        free_block(block[blk_id]);
        block[blk_id] = 0;
        write_block(indirected_block, vec);
    }
    else
    {
        size_t tgt_block_idx = blk_id / entries;
        size_t offset = blk_id % entries;

        free_indirected(block[tgt_block_idx], offset, depth - 1);
    }
}


size_t Ext2FS::alloc_inode(size_t preferred_group, bool directory)
{
    size_t block_groups = m_superblock.block_count / m_superblock.inodes_in_block_group +
            (m_superblock.block_count%m_superblock.inodes_in_block_group?1:0);

    size_t inode = alloc_inode_in_block_group(preferred_group, directory);
    if (inode) goto allocate;

    for (size_t i { 0 }; i < block_groups; ++i)
    {
        if (i != preferred_group)
        {
            inode = alloc_inode_in_block_group(i, directory);
            if (inode) goto allocate;
        }
    }

    return 0;

allocate:
    --m_superblock.unallocated_inodes;
    update_superblock();

    return inode;
}

size_t Ext2FS::alloc_inode_in_block_group(size_t group, bool directory)
{
    auto bgd = get_block_group(group);
    if (bgd.free_inodes_count == 0) return 0;

    auto bitmap = read_block(bgd.inode_bitmap);
    for (size_t j { 0 }; j < block_size()*8; ++j)
    {
        if (!bit_check(bitmap[j / 8], j % 8))
        {
            --bgd.free_inodes_count;
            if (directory) ++bgd.used_dirs_count;

            bit_set(bitmap[j / 8], j % 8);
            write_block(bgd.inode_bitmap, bitmap);

            write_block_group(group, bgd);

            return j + m_superblock.inodes_in_block_group*group + 1; // inodes start at 1
        }
    }

    return 0;
}

ext2::DirectoryEntry Ext2FS::create_dir_entry(size_t inode, uint8_t type, const kpp::string &name)
{
    ext2::DirectoryEntry new_entry;
    new_entry.inode = inode;
    new_entry.file_type = type;
    std::copy(name.begin(), name.end(), new_entry.name);
    new_entry.name_len = name.length();

    // update target link count
    auto info = read_inode(inode);
    ++info.links_count;
    write_inode(inode, info);

    return new_entry;
}

[[nodiscard]] kpp::expected<kpp::dummy_t, vfs::FSError> ext2_node::rename_impl(const kpp::string &s)
{
    if (parent() && dynamic_cast<ext2_node*>(parent()))
    {
        static_cast<ext2_node*>(parent())->update_dir_entry(inode, s);
    }

    return {};
}

void ext2_node::set_stat(const vfs::node::Stat &stat)
{
    ext2::Inode inode_struct = fs.read_inode(inode);

    inode_struct.type = (inode_struct.type & 0xF000) | stat.perms;
    inode_struct.access_time = stat.access_time;
    inode_struct.modification_time = stat.modification_time;
    inode_struct.creation_time = stat.creation_time;
    inode_struct.uid = stat.uid;
    inode_struct.gid = stat.gid;
    inode_struct.flags = stat.flags;

    write_inode_struct(inode_struct);
}

vfs::node::result<std::shared_ptr<vfs::node>> ext2_node::create_impl(const kpp::string & name, Type type)
{    
    auto dir = create_child(name, type);
    if (!dir) return nullptr;

    ext2::Inode dir_inode_struct = fs.read_inode(dir->inode);
    dir_inode_struct.type = (dir_inode_struct.type & 0xFFF) | (uint16_t)vfs_type_to_ext2_inode(type);

    dir->write_inode_struct(dir_inode_struct);

    if (type == Directory)
    {
        std::vector<ext2::DirectoryEntry> entries;

        entries.emplace_back(fs.create_dir_entry(dir->inode, (uint8_t)ext2::DirectoryType::Directory, "." ));
        entries.emplace_back(fs.create_dir_entry(inode,      (uint8_t)ext2::DirectoryType::Directory, ".."));

        fs.write_directory_entries(dir->inode, entries);
    }

    return dir;
}


kpp::expected<kpp::dummy_t, vfs::FSError> ext2_node::write_impl(size_t offset, gsl::span<const uint8_t> data)
{
    if (size() >= 65536)
    {
        fs.error("Files more than 64MiB long aren't supported\n");
        return kpp::make_unexpected(vfs::FSError{vfs::FSError::TooLarge});
    }

    fs.write_data(data, offset, fs.read_inode(inode));

    return {};
}

vfs::node::result<kpp::dummy_t> ext2_node::resize_impl(size_t size)
{
    fs.resize_inode(inode, size); // TODO : checks here
    return {};
}

vfs::node::result<kpp::dummy_t> ext2_node::remove_impl(const vfs::node * node)
{
    // remove node's children
    if (node->type() == Directory)
    {
        auto children = node->readdir();
        for (auto& child : children)
        {
            ((vfs::node*)node)->remove(child.get());
            child.reset();
        }
    }

    remove_child(node->name()); // again, checks...

    return {};
}

void ext2_node::update_dir_entry(size_t inode, const kpp::string &name)
{
    ext2::Inode inode_struct = fs.read_inode(this->inode);

    const size_t blocks = fs.data_blocks(inode_struct);

    auto entries = fs.read_directory(fs.read_data(inode_struct, 0, blocks));

    auto entry = std::find_if(entries.begin(), entries.end(), [inode](const ext2::DirectoryEntry& e)
    {
        return e.inode == inode;
    });

    if (entry == entries.end())
    {
        fs.error(("Can't find inode "+ kpp::to_string(inode) +" of directory "
                 + kpp::to_string(this->inode) +"!\n").c_str());
        return;
    }

    std::copy(name.begin(), name.end(), entry->name);
    entry->name_len = name.length();

    fs.write_directory_entries(this->inode, entries);
}

void ext2_node::write_inode_struct(const ext2::Inode& inode_struct)
{
    fs.write_inode(inode, inode_struct);
}

std::shared_ptr<ext2_node> ext2_node::create_child(const kpp::string &name, Type type)
{
    ext2::Inode inode_struct = fs.read_inode(inode);

    const size_t blocks = fs.data_blocks(inode_struct);

    auto dir_entries = fs.read_directory(fs.read_data(inode_struct, 0, blocks));

    const size_t block_group = inode / fs.m_superblock.inodes_in_block_group;
    const size_t free_inode = fs.alloc_inode(block_group, type == Directory);
    if (!free_inode) return nullptr;

    const uint8_t file_type = (uint8_t)vfs_type_to_ext2_dir(type);

    dir_entries.emplace_back(fs.create_dir_entry(free_inode, file_type, name));
    fs.write_directory_entries(inode, dir_entries);

    ext2::Inode child_inode_struct;
    memset(&child_inode_struct, 0, sizeof(ext2::Inode));

    child_inode_struct.access_time = child_inode_struct.creation_time = child_inode_struct.modification_time = Time::epoch();
    child_inode_struct.links_count = 1;
    child_inode_struct.type = 0xFFF;

    fs.write_inode(free_inode, child_inode_struct);

    auto node = std::make_shared<ext2_node>(fs, this, name, free_inode);

    return node;
}

void ext2_node::remove_child(const kpp::string& name)
{
    ext2::Inode inode_struct = fs.read_inode(inode);

    const size_t blocks = fs.data_blocks(inode_struct);

    auto dir_entries = fs.read_directory(fs.read_data(inode_struct, 0, blocks));

    for (const auto& entry : dir_entries)
    {
        if (kpp::string(entry.name, entry.name_len) == name)
        {
            fs.decrease_link_count(entry.inode);
        }
    }

    dir_entries.erase(std::remove_if(dir_entries.begin(), dir_entries.end(), [name](const ext2::DirectoryEntry& entry)
    {
        return kpp::string(entry.name, entry.name_len) == name;
    }), dir_entries.end());

    fs.write_directory_entries(inode, dir_entries);
}
