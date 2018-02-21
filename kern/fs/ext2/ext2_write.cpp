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

void Ext2FS::write_directory_entries(const ext2::Inode &inode, gsl::span<const ext2::DirectoryEntry> entries)
{

    // TODO : enlever cet appel dans read_directory
}

void ext2_node::rename_impl(const std::string &s)
{
    if (parent())
    {
        static_cast<ext2_node*>(parent())->update_dir_entry(inode, s,
                                                            (uint8_t)(is_dir() ? ext2::DirectoryType::Directory : ext2::DirectoryType::Regular));
    }
}

void ext2_node::update_dir_entry(size_t inode, const std::string &name, uint8_t type)
{
    // TODO

}
