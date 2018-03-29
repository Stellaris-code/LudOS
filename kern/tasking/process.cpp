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

#include "syscalls/syscalls.hpp"

Process::Process()
{
    init_default_fds();
}

void Process::reset(gsl::span<const uint8_t> code_to_copy, size_t allocated_size)
{
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

void Process::release_allocated_pages()
{
    for (const auto& pair : allocated_pages)
    {
        sys_free_pages(pair.first, pair.second);
    }

    allocated_pages.clear();
}

uint32_t Process::find_free_pid()
{
    for (size_t i { 0 }; i < m_processes.size(); ++i)
    {
        if (!m_processes[i])
        {
            return i;
        }
    }

    return m_processes.size();
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

bool Process::check_perms(uint16_t perms, uint16_t tgt_uid, uint16_t tgt_gid, AccessRequestPerm type)
{
    uint16_t user_flag  = (type == AccessRequestPerm::Read ? vfs::UserRead :
                                                             type == AccessRequestPerm::Write? vfs::UserWrite:
                                                                                               vfs::UserExec);
    uint16_t group_flag = (type == AccessRequestPerm::Read ? vfs::GroupRead :
                                                             type == AccessRequestPerm::Write? vfs::GroupWrite:
                                                                                               vfs::GroupExec);
    uint16_t other_flag = (type == AccessRequestPerm::Read ? vfs::OtherRead :
                                                             type == AccessRequestPerm::Write? vfs::OtherWrite:
                                                                                               vfs::OtherExec);

    if (this->uid == Process::root_uid)
    {
        return true;
    }

    if ((tgt_uid == this->uid && perms & user_flag) ||
            (tgt_gid == this->gid && perms & group_flag)||
            perms & other_flag)
    {
        return true;
    }

    return false;
}

Process::~Process()
{
    stop();

    kfree(arch_data);
}

bool Process::enabled()
{
    return m_current_process != nullptr;
}

Process &Process::current()
{
    assert(enabled());

    return *m_current_process;
}

void Process::kill(uint32_t pid)
{
    assert(by_pid(pid));

    m_processes[pid].release();
    assert(!by_pid(pid));
}

Process *Process::by_pid(uint32_t pid)
{
    if (pid >= m_processes.size())
    {
        return nullptr;
    }

    return m_processes[pid].get();
}

Process *Process::create(gsl::span<const std::string> args)
{
    uint32_t free_idx = find_free_pid();
    if (free_idx == m_processes.size())
    {
        m_processes.emplace_back(new Process);
    }
    else
    {
        m_processes[free_idx].reset(new Process);
    }

    if (m_processes[free_idx])
    {
        m_processes[free_idx]->pid = free_idx;
        m_processes[free_idx]->set_args(args);
    }

    return m_processes[free_idx].get();
}
