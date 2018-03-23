/*
process.cpp

Copyright (c) 06 Yann BOUCHER (yann)

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

#include "process.hpp"

#include "fs/vfs.hpp"

Process::Process(gsl::span<const uint8_t> code_to_copy, size_t allocated_size)
    : id(0)
{
    init_default_fds();

    arch_init(code_to_copy, allocated_size);
}

Process::Process(const std::string& _name, gsl::span<const uint8_t> code_to_copy, size_t allocated_size)
    : name(_name), id(0)
{
    init_default_fds();

    arch_init(code_to_copy, allocated_size);
}

void Process::init_default_fds()
{
    assert(vfs::find("/dev/stdout"));
    assert(vfs::find("/dev/stdin"));
    assert(vfs::find("/dev/stderr"));

    assert(add_fd({vfs::find("/dev/stdin" ), .read = true,  .write = false}) == 0);
    assert(add_fd({vfs::find("/dev/stdout"), .read = false, .write = true }) == 1);
    assert(add_fd({vfs::find("/dev/stderr"), .read = false, .write = true }) == 2);
}

size_t Process::add_fd(const FDInfo &info)
{
    // Search for an empty entry in the table
    for (size_t i { 0 }; i < fd_table.size(); ++i)
    {
        if (fd_table[i].node == nullptr)
        {
            fd_table[i] = info;
            return i;
        }
    }

    // If nothing is found, we append an entry to the table
    fd_table.emplace_back(info);
    return fd_table.size() - 1;
}

Process::FDInfo *Process::get_fd(size_t fd)
{
    if (fd >= fd_table.size())
    {
        return nullptr;
    }
    if (fd_table[fd].node == nullptr)
    {
        return nullptr;
    }

    return &fd_table[fd];
}

void Process::close_fd(size_t fd)
{
    assert(get_fd(fd));

    fd_table[fd].node = nullptr;
}
