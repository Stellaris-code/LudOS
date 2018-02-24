/*
devfs.hpp

Copyright (c) 05 Yann BOUCHER (yann)

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
#ifndef DEVFS_HPP
#define DEVFS_HPP

#include "fs/vfs.hpp"

class Disk;

namespace devfs
{
struct disk_file : public vfs::node
{
    disk_file(Disk& disk, const std::string& node_name)
        : m_disk(disk)
    {
        m_name = node_name;
    }

    virtual size_t size() const override;
    virtual bool is_dir() const override { return false; }
    Disk& disk() { return m_disk; }

protected:
    [[nodiscard]] virtual MemBuffer read_impl(size_t offset, size_t size) const override;
    [[nodiscard]] virtual bool write_impl(size_t offset, gsl::span<const uint8_t> data) override;

public:
    Disk& m_disk;
};

void init();

namespace detail
{
void add_drive(Disk& disk);
}
}

#endif // DEVFS_HPP
