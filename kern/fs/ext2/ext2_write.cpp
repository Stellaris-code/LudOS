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

void Ext2FS::umount()
{
    m_superblock.fs_state = (int)ext2::FSState::Clean;

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
            if (i % 3 == 0 || i % 5 == 0 || i % 7 == 0) check = true;
        }

        if (check)
        {
            write_block(i * m_superblock.blocks_in_block_group + 1, {(uint8_t*)&m_superblock, sizeof(ext2::Superblock)});
        }
    }
}

void Ext2FS::write_data(gsl::span<const uint8_t> data, const ext2::Inode &inode, size_t offset, size_t size)
{
    size_t blocks = inode.blocks_512 / (block_size()/512) + (inode.blocks_512%(block_size()/512)?1:0);

    assert(offset + size <= blocks);

    for (size_t i { offset }; i < offset + size; ++i)
    {
        const size_t data_offset = i-offset;
        write_data_block(data.subspan(data_offset*block_size(), block_size()),
                         inode, i);
    }
}

void Ext2FS::write_data_block(gsl::span<const uint8_t> data, const ext2::Inode &inode, size_t blk_id)
{
    assert(data.size() <= block_size());

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
    assert(data.size() <= block_size());

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

void Ext2FS::resize_inode(size_t inode, size_t size)
{

}

void Ext2FS::write_directory_entries(size_t inode, gsl::span<const ext2::DirectoryEntry> entries)
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
        //if (cursor != 0) assert(cursor % block_size()); // not at end of block
        assert(entry.record_len >= 8 + entry.name_len);
        assert(entry.inode);

        std::copy((uint8_t*)&entry, (uint8_t*)&entry + 8 + entry.name_len, data.begin() + cursor);

        cursor += entry.record_len;

        assert(cursor % 4 == 0);
    }
    assert(cursor == data.size());

//    auto org_data = read_data(read_inode(inode), 0, data.size()/block_size());
//    if (memcmp(org_data.data(), data.data(), data.size()) != 0)
//    {
//        dump(data.data(), data.size());
//    }

    //    assert(memcmp(read_data(read_inode(inode), 0, 1).data(), data.data(), data.size()) == 0);
    //    dump(read_data(read_inode(inode), 0, 1).data(), 100);
    //    dump(data.data(), 100);

    write_data(data, read_inode(inode), 0, data.size()/block_size());
    // TODO : enlever cet appel dans read_directory
}

void ext2_node::rename_impl(const std::string &s)
{
    if (parent() && dynamic_cast<ext2_node*>(parent()))
    {
        static_cast<ext2_node*>(parent())->update_dir_entry(inode, s,
                                                            (uint8_t)(is_dir() ? ext2::DirectoryType::Directory : ext2::DirectoryType::Regular));
    }
}

void ext2_node::set_stat(const vfs::node::Stat &stat)
{

}

void ext2_node::update_dir_entry(size_t inode, const std::string &name, uint8_t type)
{
    // TODO

}
