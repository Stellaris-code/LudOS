/*
ext2_structures.hpp

Copyright (c) 25 Yann BOUCHER (yann)

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
#ifndef EXT2_STRUCTURES_HPP
#define EXT2_STRUCTURES_HPP

#include <stdint.h>

namespace ext2
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
    int32_t frag_size; // size = 1024 << frag_size
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
    // Revision >= 1
    uint32_t first_inode;
    uint16_t inode_size;
    uint16_t superblock_block_group;
    uint32_t optional_features;
    uint32_t required_features;
    uint32_t ro_required_features;
    uint8_t fs_id[16];
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
    uint32_t hash_seeds[4];
    uint8_t default_hash_version;
    uint32_t default_mount_options;
    uint32_t first_meta_block_gid;
    uint8_t unused1[760];
};
static_assert(sizeof(Superblock) == 1024);

inline constexpr uint16_t signature = 0xef53;

enum class FSState : uint16_t
{
    Clean = 1,
    HasErrors = 2
};

enum class ErrorHandling : uint16_t
{
    Ignore = 1,
    Remount = 2,
    Panic = 3
};

enum class OptFeatureFlags : uint32_t
{
    DirPreallocateBlocks = 0x1,
    AFSServerInodes = 0x2,
    HasJournal = 0x4,
    InodeExtendedAttributes = 0x8,
    FSCanResizeItself = 0x10,
    DirectoriesUseHashIndex = 0x20
};

enum class RequiredFeatureFlags : uint32_t
{
    Compression = 0x1,
    DirectoriesHaveATypeField = 0x2,
    FSNeedsJournalReplay = 0x4,
    FSUsesJournal = 0x8
};

enum class ReadOnlyFeatureFlags : uint32_t
{
    SparseSuperblocks = 0x1,
    FS64File = 0x2,
    BinaryTreeDirContents = 0x4
};

struct BlockGroupDescriptor
{
    uint32_t block_bitmap;
    uint32_t inode_bitmap;
    uint32_t inode_table;
    uint16_t free_blocks_count;
    uint16_t free_inodes_count;
    uint16_t used_dirs_count;
    uint16_t pad;
    uint8_t reserved[12];
};
static_assert(sizeof(BlockGroupDescriptor) == 32);

struct Inode
{
    uint16_t type;
    uint16_t uid;
    uint32_t size_lower;
    uint32_t access_time;
    uint32_t creation_time;
    uint32_t modification_time;
    uint32_t deletion_time;
    uint16_t gid;
    uint16_t links_count;
    uint32_t blocks_512;
    uint32_t flags;
    uint32_t osd1;
    uint32_t block_ptr[15];
    uint32_t generation;
    uint32_t file_acl;
    uint32_t dir_acl;
    uint32_t frag_addr;
    uint8_t osd2[12];
};
static_assert(sizeof(Inode) == 128);

enum class ReservedInodes : uint32_t
{
    BadInode = 1,
    RootInode = 2,
    AclIdxInode = 3,
    AclDataInode = 4,
    BootloaderInode = 5,
    UndeleteInode = 6
};

enum class InodeType : uint16_t
{
    Socket = 0xC000,
    Symlink = 0xA000,
    Regular = 0x8000,
    BlockDev = 0x6000,
    Directory = 0x4000,
    CharDev = 0x2000,
    Fifo = 0x1000
};
enum class InodeTypeFlags : uint16_t
{
    SUID = 0x0800,
    SGID = 0x0400,
    StickyBit = 0x0200
};
enum class InodePerms : uint16_t
{
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

enum class InodeFlags : uint32_t
{
    SecureDel = 0x1,
    UndelRecord = 0x2,
    Compressed = 0x4,
    SyncUpdates = 0x8,
    Immutable = 0x10,
    AppendOnly = 0x20,
    NoDump = 0x40,
    NoAccessTimeUpdate = 0x80,
    Dirty = 0x100,
    CompressedBlocks = 0x200,
    NoCompression = 0x400,
    CompressionError = 0x800,
    HashIndexed = 0x1000,
    AFSDirectory = 0x2000,
    JournalData = 0x4000,
    Reserved = 0x80000000
};

struct DirectoryEntry
{
    uint32_t inode;
    uint16_t record_len;
    uint8_t name_len;
    uint8_t file_type;
    char name[255];
};

enum class DirectoryType : uint8_t
{
    Unknown = 0,
    Regular = 1,
    Directory = 2,
    CharDev = 3,
    BlockDev = 4,
    Fifo = 5,
    Socket = 6,
    Symlink = 7
};

struct IndexedDirectoryRoot
{
    uint32_t reserved;
    uint8_t hash_versiion;
    uint8_t info_length;
    uint8_t indirect_levels;
    uint8_t reserved2;
};

enum class HashVersion : uint8_t
{
    Legacy = 0,
    HalfMD4 = 1,
    Tea = 2
};

struct IndexedDirectoryEntry
{
    uint32_t hash;
    uint32_t block;
};
struct IndexedDirectoryEntryCount
{
    uint16_t limit;
    uint16_t count;
};

struct ExtendedAttributeBlockHeader
{
    uint32_t magic;
    uint32_t refcount;
    uint32_t blocks;
    uint32_t hash;
    uint8_t reserved[16];
};
inline constexpr uint32_t extended_attribute_magic = 0xEA020000;

struct AttributeEntryHeader
{
    uint8_t name_len;
    uint8_t name_index;
    uint16_t value_offs;
    uint32_t value_block;
    uint32_t value_size;
    uint32_t hash;
    char name[];
};

};

#endif // EXT2_STRUCTURES_HPP
