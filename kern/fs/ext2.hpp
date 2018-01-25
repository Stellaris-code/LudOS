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

#include "fs.hpp"

#include <optional.hpp>

class Ext2FS : public FSImpl<Ext2FS>
{
    struct Superblock
    {
        uint32_t inode_count;
        uint32_t block_count;
        uint32_t superuser_blocks;
        uint32_t unallocated_blocks;
        uint32_t unallocated_inodes;
        uint32_t superblock_block_number;
        uint32_t block_size; // size = 1024 << block_size
        uint32_t frag_size; // size = 1024 << frag_size
        uint32_t blocks_in_block_group;
        uint32_t frags_in_block_group;
        uint32_t inodes_in_block_group;
        uint32_t last_mount_time;
        uint32_t last_write_time;
        uint16_t mounts_since_fsck;
        uint16_t mounts_before_fsck;
        uint16_t ext2_signature;
        uint16_t fs_state;
        uint16_t error_handling;
        uint16_t version_minor;
        uint32_t last_fsck_time;
        uint32_t forced_fsck_interval;
        uint32_t os_id;
        uint32_t version_major;
        uint16_t reserved_blocks_uid;
        uint16_t reserved_blocks_gid;
        uint32_t first_inode;
        uint16_t inode_size;
        uint16_t superblock_block_group;
        uint32_t optional_features;
        uint32_t required_features;
        uint32_t write_required_features;
        char fs_id[16];
        char volume_name[16];
        char last_mount_path[64];
        uint32_t used_compression_algorithms;
        uint8_t preallocated_blocks_file;
        uint8_t preallocated_blocks_dir;
        uint16_t unused0;
        char journal_id[16];
        uint32_t journal_inode;
        uint32_t journal_device;
        uint32_t orphan_inode_head;
        uint8_t unused1[787];
    };
    static_assert(sizeof(Superblock) == 1024);

    enum FSState : uint16_t
    {
        Clean = 1,
        HasErrors = 2
    };

    enum ErrorHandling : uint16_t
    {
        Ignore = 1,
        Remount = 2,
        Panic = 3
    };

    enum OptFeatureFlags : uint32_t
    {
        DirPreallocateBlocks = 0x1,
        AFSServerInodes = 0x2,
        HasJournal = 0x4,
        InodeExtendedAttributes = 0x8,
        FSCanResizeItself = 0x10,
        DirectoriesUseHashIndex = 0x20
    };

    enum RequiredFeatureFlags : uint32_t
    {
        Compression = 0x1,
        DirectoriesHaveATypeField = 0x2,
        FSNeedsJournalReplay = 0x4,
        FSUsesJournal = 0x8
    };

    enum ReadOnlyFeatureFlags : uint32_t
    {
        SparseSuperblocks = 0x1,
        FS64File = 0x2,
        BinaryTreeDirContents = 0x4
    };

    static constexpr uint16_t ext2_signature = 0xef53;

public:
    Ext2FS(Disk& disk);

    static bool accept(const Disk& disk);

private:
    static std::optional<const Superblock> read_superblock(const Disk& disk);
};

#endif // EXT2_HPP
