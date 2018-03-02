/*
fs.hpp

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
#ifndef FS_HPP
#define FS_HPP

#include "drivers/storage/disk.hpp"
#include "vfs.hpp"

#include "utils/vecutils.hpp"

class FileSystem
{
public:
    FileSystem(Disk& disk)
        : m_disk(disk)
    {}

    virtual std::shared_ptr<vfs::node> root() const = 0;

    virtual std::string type() const = 0;
    virtual size_t total_size() const = 0;
    virtual size_t free_size() const = 0;

    virtual void umount() = 0;

    const Disk& disk() const { return m_disk; }

    static bool accept(const Disk& disk) { assert(false); return false; }

    static FileSystem *get_disk_fs(Disk& disk);

public:
    static ref_vector<FileSystem> fs_list()
    {
        ref_vector<FileSystem> vec;
        for (auto& fs : m_fs_list)
        {
            vec.emplace_back(*fs);
        }

        return vec;
    }

protected:
    Disk& m_disk;

protected:
    static inline std::vector<std::unique_ptr<FileSystem>> m_fs_list;
};

template <typename Derived>
class FSImpl : public FileSystem
{
public:
    FSImpl(Disk& disk) : FileSystem(disk)
    {}

public:
    template <typename... Args>
    static Derived& create_disk(Args&&... args)
    {
        static_assert(std::is_base_of_v<FileSystem, Derived>);
        FileSystem::m_fs_list.emplace_back(std::make_unique<Derived>(std::forward<Args>(args)...));
        return static_cast<Derived&>(FileSystem::fs_list().back().get());
    }
};

namespace fs::detail
{
    using FsEntry = FileSystem*(*)(Disk& disk);

    constexpr size_t max_fs { 0x2000 };

    extern FsEntry fs[max_fs];
    extern FsEntry* fs_list_ptr;
}

#define ADD_FS(name) \
__attribute__((constructor)) void _fs_init_##name() \
{ \
    *fs::detail::fs_list_ptr++ = [](Disk& disk)->FileSystem* \
    { \
        if (name::accept(disk)) \
        { \
            return &name::create_disk(disk); \
        } \
        return nullptr; \
    }; \
}

#endif // FS_HPP
