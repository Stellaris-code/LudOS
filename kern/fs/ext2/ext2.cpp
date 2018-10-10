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
#include "utils/mathutils.hpp"
#include "utils/stlutils.hpp"

#include "fs/fsutils.hpp"
#include "fs/vfs.hpp"

Ext2FS::Ext2FS(Disk &disk) : FSImpl<Ext2FS>(disk)
{
    m_superblock = *read_superblock(disk);

    // these are the only supported features
    m_superblock.optional_features = (int)ext2::OptFeatureFlags::InodeExtendedAttributes | (int)ext2::OptFeatureFlags::InodeResize;

    m_current_block.resize(block_size());

    check_superblock_backups();

    m_superblock.last_mount_time = Time::epoch();
    ++m_superblock.mounts_since_fsck;
    m_has_error = m_superblock.fs_state;
    m_superblock.fs_state = (uint16_t)ext2::FSState::HasErrors;

    update_superblock();
}

bool Ext2FS::accept(const Disk &disk)
{
    auto superblock = read_superblock(disk);
    if (!superblock) return false;

    return check_superblock(*superblock);
}

std::shared_ptr<vfs::node> Ext2FS::root() const
{
    std::shared_ptr<ext2_node> ptr = std::make_shared<ext2_node>(*(Ext2FS*)this, nullptr, "", 2);
    assert(ptr->type() == vfs::node::Directory);
    return ptr;
}

kpp::optional<const ext2::Superblock> Ext2FS::read_superblock(const Disk &disk)
{
    if (disk.disk_size() < 1024*2)
    {
        return {};
    }

    auto data = disk.read(1024, 1024);
    if (!data) return {};
    return *reinterpret_cast<const ext2::Superblock*>(data->data());
}

bool Ext2FS::check_superblock(const ext2::Superblock &superblock)
{
    if (superblock.ext2_signature != ext2::signature)
    {
        return false;
    }

    log(Notice, "Ext2Fs revision %d.%d\n", superblock.version_major, superblock.version_minor);

    if (superblock.mounts_since_fsck + 1 >= superblock.mounts_before_fsck) // not updated yet, add one
    {
        warn("Mounts : fsck should be run ! (mounted %d/%d times)\n", superblock.mounts_since_fsck+1, superblock.mounts_before_fsck);
    }
    if (superblock.fs_state == (int)ext2::FSState::HasErrors)
    {
        warn("Filesystem is not clean, fsck should be run !\n");
    }

    if (superblock.forced_fsck_interval && Time::epoch() - superblock.last_fsck_time >= superblock.forced_fsck_interval)
    {
        warn("Time : fsck should be run ! (last check time : %s, should have run check on %s)\n", Time::to_string(Time::to_local_time(Time::from_unix(superblock.last_fsck_time))),
             Time::to_string(Time::to_local_time(Time::from_unix(superblock.last_fsck_time + superblock.forced_fsck_interval))));
    }

    if (superblock.version_major >= 1)
    {
        if (superblock.required_features & (int)ext2::RequiredFeatureFlags::Compression ||
                superblock.required_features & (int)ext2::RequiredFeatureFlags::FSNeedsJournalReplay ||
                superblock.required_features & (int)ext2::RequiredFeatureFlags::FSUsesJournal)
        {
            warn("Filesystem requires unsupported features : 0x%x\n", superblock.required_features);
            return false;
        }

        if (superblock.ro_required_features & (int)ext2::ReadOnlyFeatureFlags::BinaryTreeDirContents)
        {
            warn("Filesystem requires unsupported features : 0x%x (ro)\n", superblock.ro_required_features);
            return false;
        }
    }

    log(Notice, "Ext2FS optional features : 0x%x\n", superblock.optional_features);

    return true;
}

bool Ext2FS::check_superblock_backups() const
{
    const size_t last_group = m_superblock.block_count / m_superblock.blocks_in_block_group;

    bool okay = true;

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
            read_block(i * m_superblock.blocks_in_block_group + 1, m_current_block);

            log(Debug, "block : %d, offset 0x%x (%d)\n", i * m_superblock.blocks_in_block_group + 1,
                (i * m_superblock.blocks_in_block_group + 1) * block_size(),
                (i * m_superblock.blocks_in_block_group + 1) * block_size());

            if (((ext2::Superblock*)m_current_block.data())->ext2_signature != ext2::signature)
            {
                warn("Superblock backup at group %d has an invalid signature\n", i);
                okay = false;
            }
        }
    }

    return okay;
}

size_t Ext2FS::block_group_table_block() const
{
    return m_superblock.block_size == 0 ? 2 : 1;
}

const ext2::BlockGroupDescriptor Ext2FS::get_block_group(size_t idx) const
{
    const size_t group_desc_per_block = block_size()/sizeof(ext2::BlockGroupDescriptor);

    const size_t block_to_read = block_group_table_block() + (idx / group_desc_per_block);

    read_block(block_to_read, m_current_block);

    return ((ext2::BlockGroupDescriptor*)m_current_block.data())[idx % group_desc_per_block];
}

kpp::expected<kpp::dummy_t, vfs::FSError> Ext2FS::read_block(size_t number, gsl::span<uint8_t> data) const
{
    assert(number < m_superblock.block_count);
    assert(data.size() == block_size());

    auto result = m_disk.read(number * block_size(), data);
    if (!result)
    {
        return kpp::make_unexpected(vfs::FSError{vfs::FSError::ReadError, {result.error().type}});
    }

    return {};
}

const ext2::Inode Ext2FS::read_inode(size_t inode) const
{
    if (!check_inode_presence(inode))
    {
        error(("Inode " + kpp::to_string(inode) + " is marked as free\n").c_str());
    }

    auto block_group = get_block_group((inode - 1) / m_superblock.inodes_in_block_group);

    size_t index = (inode - 1) % m_superblock.inodes_in_block_group;
    size_t block_idx = (index * inode_size()) / block_size();
    size_t offset = index % (block_size() / inode_size());

    read_block(block_group.inode_table + block_idx, m_current_block);

    return ((ext2::Inode*)m_current_block.data())[offset];
}

std::vector<ext2::DirectoryEntry> Ext2FS::read_directory_entries(size_t inode) const
{
    std::vector<ext2::DirectoryEntry> vec;

    auto inode_struct = read_inode(inode);

    size_t blocks = data_blocks(inode_struct);

    MemBuffer buf(blocks*block_size());
    auto result = read_data(inode_struct, 0, buf);
    assert(result); // TODO
    return read_directory(buf);
}

bool Ext2FS::check_inode_presence(size_t inode) const
{
    auto block_group = get_block_group((inode - 1) / m_superblock.inodes_in_block_group);

    size_t index = (inode - 1) % m_superblock.inodes_in_block_group;

    read_block(block_group.inode_bitmap, m_current_block);

    return bit_check(m_current_block[index / 8], index % 8);
}

size_t Ext2FS::data_blocks(const ext2::Inode &inode) const
{
    return inode.size_lower / block_size() + (inode.size_lower%block_size()?1:0);
}

size_t Ext2FS::count_used_blocks(const ext2::Inode &inode) const
{
    const size_t entries_per_block = block_size()/sizeof(uint32_t);
    size_t blk = data_blocks(inode); // start

    // TODO

    if (data_blocks(inode) >= 12)
    {
        blk += 1;
    }

    if ((data_blocks(inode) - 12) >= entries_per_block)
    {
        blk += (data_blocks(inode) - 12) / entries_per_block +
                ((data_blocks(inode) - 12)%entries_per_block?1:0);
    }

    return blk;
}

size_t Ext2FS::get_data_block(const ext2::Inode &inode, size_t blk_id) const
{
    const size_t entries_per_block = block_size()/sizeof(uint32_t);

    if (blk_id < 12)
    {
        return inode.block_ptr[blk_id];
    }
    blk_id -= 12;

    if (blk_id < entries_per_block)
    {
        return get_data_block_indirected(inode.block_ptr[12], blk_id, 1);
    }
    blk_id -= entries_per_block;

    if (blk_id < entries_per_block*entries_per_block)
    {
        return get_data_block_indirected(inode.block_ptr[13], blk_id, 2);
    }
    blk_id -= entries_per_block*entries_per_block;

    return get_data_block_indirected(inode.block_ptr[14], blk_id, 3);
}

size_t Ext2FS::get_data_block_indirected(size_t indirected_block, size_t blk_id, size_t depth) const
{
    MemBuffer data;
    size_t entries = ipow<size_t>(block_size()/sizeof(uint32_t), depth-1);
    read_block(indirected_block, m_current_block);
    uint32_t* block = (uint32_t*)m_current_block.data();

    if (depth <= 1) return block[blk_id];
    else
    {
        size_t tgt_block_idx = blk_id / entries;
        size_t offset = blk_id % entries;

        return get_data_block_indirected(block[tgt_block_idx], offset, depth - 1);
    }
}

kpp::error<vfs::FSError> Ext2FS::read_data_block(const ext2::Inode& inode, size_t blk_id, gsl::span<uint8_t> data) const
{
    const size_t entries_per_block = block_size()/sizeof(uint32_t);

    if (blk_id < 12)
    {
        return read_block(inode.block_ptr[blk_id], data);
    }
    blk_id -= 12;

    if (blk_id < entries_per_block)
    {
        return read_indirected(inode.block_ptr[12], blk_id, 1, data);
    }
    blk_id -= entries_per_block;

    if (blk_id < entries_per_block*entries_per_block)
    {
        return read_indirected(inode.block_ptr[13], blk_id, 2, data);
    }
    blk_id -= entries_per_block*entries_per_block;

    return read_indirected(inode.block_ptr[14], blk_id, 3, data);
}

kpp::error<vfs::FSError> Ext2FS::read_indirected(size_t indirected_block, size_t blk_id, size_t depth, gsl::span<uint8_t> data) const
{
    size_t entries = ipow<size_t>(block_size()/sizeof(uint32_t), depth-1);
    read_block(indirected_block, m_current_block);
    uint32_t* block = (uint32_t*)m_current_block.data();

    if (depth <= 1)
    {
        return read_block(block[blk_id], data);
    }
    else
    {
        size_t tgt_block_idx = blk_id / entries;
        size_t offset = blk_id % entries;

        return read_indirected(block[tgt_block_idx], offset, depth - 1, data);
    }
}

kpp::error<vfs::FSError> Ext2FS::read_data(const ext2::Inode &inode, size_t offset, gsl::span<uint8_t> data) const
{
    size_t blocks = data_blocks(inode);

    assert(data.size() % block_size() == 0);
    assert(offset + data.size()/block_size() <= blocks);

    for (size_t i { offset }; i < offset + data.size()/block_size(); ++i)
    {
        auto result = read_data_block(inode, i, {data.data() + i*block_size(), data.size()});
        if (!result) return result;
    }

    return {};
}

std::vector<ext2::DirectoryEntry> Ext2FS::read_directory(gsl::span<const uint8_t> data) const
{
    std::vector<ext2::DirectoryEntry> entries;
    entries.reserve(255);
    const uint8_t* ptr = (const uint8_t*)data.data();

    while (ptr < data.data() + data.size())
    {
        if (((const ext2::DirectoryEntry*)ptr)->inode) entries.emplace_back(*(const ext2::DirectoryEntry*)ptr);

        if (((const ext2::DirectoryEntry*)ptr)->record_len == 0)
        {
            error("Invalid directory record length!\n");
            return entries;
        }

        ptr += ((const ext2::DirectoryEntry*)ptr)->record_len;
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

void Ext2FS::error(const kpp::string &message) const
{
    m_has_error = (uint16_t)ext2::FSState::HasErrors;
    if (m_superblock.error_handling == (int)ext2::ErrorHandling::Panic)
    {
        panic("%s", message.c_str());
    }
    else
    {
        warn("%s", message.c_str());
    }
}

kpp::string Ext2FS::link_name(const ext2::Inode &inode_struct)
{
    kpp::string str;

    if (inode_struct.size_lower <= 60)
    {
        str = kpp::string((const char*)inode_struct.block_ptr, 60);
    }
    else
    {
        if (!read_data(inode_struct, 0, {m_current_block.data(), (long)inode_struct.size_lower}))
            return "<invalid>";

        str = kpp::string((const char*)m_current_block.data(), inode_struct.size_lower);
    }
    str += '\0'; // just to be safe

    return trim_zstr(str);
}

kpp::expected<size_t, vfs::FSError> ext2_node::read_impl(size_t offset, gsl::span<uint8_t> data) const
{
    if (this->size() >= 65536)
    {
        fs.error("Files more than 64MiB long aren't supported\n");
    }

    if (is_link())
    {
        auto ptr = link_target();
        if (ptr) return ptr->read(offset, data);
        else return kpp::make_unexpected(vfs::FSError{vfs::FSError::InvalidLink});
    }

    const auto& inode_struct = fs.read_inode(inode);

#if 0
    size_t block_off = offset/fs.block_size();
    size_t block_size = data.size()/fs.block_size() + (data.size()%fs.block_size()?1:0);

    size_t byte_off = offset % fs.block_size();

    MemBuffer buffer(block_size*fs.block_size());
    auto result = fs.read_data(inode_struct, block_off, buffer);
    if (!result) return kpp::make_unexpected(result.error());

    std::copy(buffer.begin() + byte_off, buffer.begin() + offset + data.size(), data.begin());

    return (offset + data.size()) - byte_off;
#else
    const size_t blk_size = fs.block_size();

    const size_t first_stride_size = offset%blk_size;
    const size_t second_stride_size = (offset+data.size()) % blk_size;
    const size_t mid_stride_size = data.size() - ((blk_size - first_stride_size)%blk_size) - second_stride_size;
    if (first_stride_size != 0) // offset isn't aligned, manually fetch the first sector
    {
        MemBuffer buf(blk_size);
        auto result = fs.read_data_block(inode_struct, offset/blk_size, buf);
        if (!result) return kpp::make_unexpected(result.error());
        std::copy(buf.begin() + first_stride_size, buf.begin() + blk_size, data.begin());
    }
    if (second_stride_size != 0) // size isn't aligned, manually fetch the last sector
    {
        MemBuffer buf(blk_size);
        auto result = fs.read_data_block(inode_struct, (offset+data.size())/blk_size, buf);
        if (!result) return kpp::make_unexpected(result.error());
        std::copy(buf.begin(), buf.begin() + second_stride_size, data.begin() + (data.size()-second_stride_size));
    }

    if (mid_stride_size != 0)
    {
        gsl::span<uint8_t> aligned_span = {data.data() + (blk_size - first_stride_size)%blk_size,
                                           (long)mid_stride_size};

        assert(aligned_span.size() % blk_size == 0);

        auto result = fs.read_data(inode_struct, offset/blk_size, aligned_span);
        if (!result) return kpp::make_unexpected(result.error());
    }
#endif
}

std::vector<std::shared_ptr<vfs::node>> ext2_node::readdir_impl()
{
    if (is_link())
    {
        auto ptr = link_target();
        if (ptr) return ptr->readdir();
        else return {};
    }

    std::vector<std::shared_ptr<vfs::node>> vec;

    for (const auto& entry : fs.read_directory_entries(inode))
    {
        auto entry_name = kpp::string(entry.name, entry.name_len);
        if (entry_name != "." &&
                entry_name != "..")
        {
            vec.push_back(std::static_pointer_cast<vfs::node>(
                              std::make_shared<ext2_node>(fs, this, entry_name, entry.inode)));
        }

    }

    return vec;
}

size_t ext2_node::size() const
{
    if (is_link())
    {
        auto ptr = link_target();
        if (ptr) return ptr->size();
        else return 0;
    }

    return fs.read_inode(inode).size_lower;
}

vfs::node::Type ext2_node::type() const
{
    if (is_link())
    {
        auto ptr = link_target();
        if (ptr) return ptr->type();
        else return Unknown;
    }

    ext2::Inode inode_struct = fs.read_inode(inode);

    switch ((ext2::InodeType)(inode_struct.type & 0xF000))
    {
        case ext2::InodeType::Regular:
            return File;
        case ext2::InodeType::Directory:
            return Directory;
        case ext2::InodeType::Symlink:
            return SymLink;
        default:
            return Unknown;
    }
}

bool ext2_node::is_link() const
{
    ext2::Inode inode_struct = fs.read_inode(inode);

    return (inode_struct.type & 0xF000) == (uint16_t)ext2::InodeType::Symlink;
}

ext2::InodeType ext2_node::vfs_type_to_ext2_inode(vfs::node::Type type)
{
    switch (type)
    {
        case Directory:
            return ext2::InodeType::Directory;
        case SymLink:
            return ext2::InodeType::Symlink;
        case File:
        default:
            return ext2::InodeType::Regular;
    }
}

ext2::DirectoryType ext2_node::vfs_type_to_ext2_dir(vfs::node::Type type)
{
    switch (type)
    {
        case Directory:
            return ext2::DirectoryType::Directory;
        case SymLink:
            return ext2::DirectoryType::Symlink;
        case File:
        default:
            return ext2::DirectoryType::Regular;
    }
}

vfs::node::Stat ext2_node::stat() const
{
    ext2::Inode inode_struct = fs.read_inode(inode);

    Stat stat;
    stat.perms = inode_struct.type & 0x0FFF;
    stat.access_time = inode_struct.access_time;
    stat.modification_time = inode_struct.modification_time;
    stat.creation_time = inode_struct.creation_time;
    stat.uid = inode_struct.uid;
    stat.gid = inode_struct.gid;
    stat.flags = inode_struct.flags;

    return stat;
}

kpp::string ext2_node::name() const
{
    return filename;
}

kpp::string ext2_node::link_name() const
{
    ext2::Inode inode_struct = fs.read_inode(inode);

    return fs.link_name(inode_struct);
}

std::shared_ptr<vfs::node> ext2_node::link_target() const
{
    assert(is_link());
    return vfs::find(m_parent->path() + link_name()).value_or(nullptr);
}

ADD_FS(Ext2FS)
