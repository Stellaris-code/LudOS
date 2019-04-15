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
#include "process_data.hpp"

#include "utils/kmsgbus.hpp"

#include "fs/fsutils.hpp"

#include "syscalls/syscalls.hpp"
#include "tasking/scheduler.hpp"

#include "mem/memmap.hpp"
#include "mem/meminfo.hpp"
#include "mem/page_fault.hpp"

#include "utils/stlutils.hpp"
#include "utils/memutils.hpp"

#include "cpu/stack.hpp"

#include "shared_memory.hpp"

#include "fs/vfs.hpp"

#include <sys/wait.h>
#include <siginfo.h>

using namespace tasking;

Process::Process()
{
    data = std::make_unique<ProcessData>();

    data->pwd = vfs::root;
    data->root = vfs::root;
    init_default_fds();
    init_sig_handlers();

    data->user_callbacks = std::make_shared<tasking::UserCallbacks>();
    data->mappings = std::make_shared<std::unordered_map<uintptr_t, tasking::MemoryMapping>>();

    arch_init();
}

void Process::init_default_fds()
{
    data->fd_table = std::make_shared<std::vector<tasking::FDInfo>>();
    assert(data->fd_table);

    static auto stdin_node = vfs::find("/dev/stdin" ).value();
    static auto stdout_node = vfs::find("/dev/stdout" ).value();
    static auto stderr_node = vfs::find("/dev/stderr" ).value();

    add_fd({stdin_node, .read = true,  .write = false});
    add_fd({stdout_node, .read = false, .write = true });
    add_fd({stderr_node, .read = false, .write = true });
}

void Process::init_sig_handlers()
{
    assert(!data->sig_handlers);
    data->sig_handlers = std::make_shared<kpp::array<struct sigaction, SIGRTMAX>>();
    for (size_t i { 0 }; i < data->sig_handlers->size(); ++i)
    {
        (*data->sig_handlers)[i].sa_handler = (sighandler_t)default_sighandler_actions[i];
    }
}

pid_t Process::find_free_pid()
{
    for (size_t i { 0 }; i < m_processes.size(); ++i)
    {
        if (!m_processes[i]) return i;
    }

    return m_processes.size();
}

size_t Process::add_fd(const tasking::FDInfo &info)
{
    // Search for an empty entry in the table
    for (size_t i { 0 }; i < data->fd_table->size(); ++i)
    {
        if ((*data->fd_table)[i].node == nullptr)
        {
            (*data->fd_table)[i] = info;
            return i;
        }
    }

    // If nothing is found, we append an entry to the table
    data->fd_table->emplace_back(info);
    return data->fd_table->size() - 1;
}

tasking::FDInfo *Process::get_fd(size_t fd)
{
    if (fd >= data->fd_table->size() || (*data->fd_table)[fd].node == nullptr)
    {
        return nullptr;
    }

    return &(*data->fd_table)[fd];
}

void Process::close_fd(size_t fd)
{
    assert(get_fd(fd));

    (*data->fd_table)[fd].node = nullptr;
}

void Process::wait_for(pid_t pid, int *wstatus)
{
    assert(Process::by_pid(pid));

    Process::by_pid(pid)->data->wait_entries.push_back({this->pid, Memory::physical_address(wstatus)});
    data->waitstatus_phys = Memory::physical_address(wstatus);

    status = ChildWait;

    tasking::schedule();
}

void Process::wake_up(pid_t awakener, int err_code)
{
    Memory::phys_write(data->waitstatus_phys, &err_code, sizeof(err_code));

    data->woke_up_by = awakener;

    status = Active;
}

bool Process::check_perms(uint16_t perms, uint16_t tgt_uid, uint16_t tgt_gid, uint16_t type)
{
    uint16_t user_flag  = (type == AccessRequestPerm::ReadRequest ? vfs::UserRead :
                                                                    type == AccessRequestPerm::WriteRequest? vfs::UserWrite:
                                                                                                             vfs::UserExec);
    uint16_t group_flag = (type == AccessRequestPerm::ReadRequest ? vfs::GroupRead :
                                                                    type == AccessRequestPerm::WriteRequest? vfs::GroupWrite:
                                                                                                             vfs::GroupExec);
    uint16_t other_flag = (type == AccessRequestPerm::ReadRequest ? vfs::OtherRead :
                                                                    type == AccessRequestPerm::WriteRequest? vfs::OtherWrite:
                                                                                                             vfs::OtherExec);

    if (data->uid == Process::root_uid)
    {
        return true;
    }

    if ((tgt_uid == data->uid && perms & user_flag) ||
            (tgt_gid == data->gid && perms & group_flag)||
            perms & other_flag)
    {
        return true;
    }

    return false;
}

void Process::raise(pid_t target_pid, int signal, const siginfo_t &siginfo)
{
    auto proc = Process::by_pid(target_pid);
    assert(proc);
    assert(signal < proc->data->sig_handlers->size());

    //log_serial("Sent signal %d to pid %d\n", signal, target_pid);

    auto sig = proc->data->sig_handlers->at(signal);
    switch ((intptr_t)sig.sa_handler)
    {
        case SIG_ACTION_IGN:
            return;
        case SIG_ACTION_CORE:
        case SIG_ACTION_TERM:
            Process::kill(target_pid, __W_EXITCODE(255, signal));
            return;
        case SIG_ACTION_STOP:
            // TODO
            return;
        case SIG_ACTION_CONT:
            // TODO
            return;
        default:
            break;
    }

    proc->execute_sighandler(signal, pid, siginfo);
}

void Process::kill(pid_t pid, int err_code)
{
    if (pid == 0)
    {
        panic("Tried to kill the master process !\n");
    }

    assert(by_pid(pid));

    siginfo_t info;
    info.si_signo = SIGCHLD;
    info.si_code = (WIFEXITED(err_code) ? CLD_EXITED : CLD_KILLED);
    info.si_pid = pid;
    info.si_uid = m_processes[pid]->data->uid;

    info.si_status = err_code;

    m_processes[pid]->status = Zombie;

    for (auto child_pid : m_processes[pid]->data->children)
    {
        by_pid(child_pid)->parent = init_pid; // reset parent of child processes
        by_pid(init_pid)->data->children.push_back(child_pid);
    }

    kmsgbus.send(ProcessDestroyedEvent{pid, err_code});

    assert(by_pid(by_pid(pid)->parent));
    auto& parent = *by_pid(by_pid(pid)->parent);

    for (auto entry : m_processes[pid]->data->wait_entries)
    {
        assert(Process::by_pid(entry.process_waiting)->status == ChildWait);
        Process::by_pid(entry.process_waiting)->wake_up(pid, err_code);
    }

    parent.raise(parent.pid, SIGCHLD, info);

    if (pid == m_current_process->pid) // if we killed the current process, do a context switch
    {
        tasking::schedule();
    }
}

void Process::release_zombie(pid_t pid)
{
    assert(m_current_process->pid != pid);
    assert(by_pid(pid)->status == Zombie);

    assert(by_pid(by_pid(pid)->parent));
    auto& parent = *by_pid(by_pid(pid)->parent);

    // erase pid from parent children list
    assert(find(parent.data->children, pid));
    erase(parent.data->children, pid);

    m_processes[pid].reset();

    m_process_count--;

    assert(by_pid(pid) == nullptr);
}

Process *Process::by_pid(pid_t pid)
{
    assert(pid >= 0);
    if ((size_t)pid >= m_processes.size())
    {
        return nullptr;
    }

    return m_processes[pid].get();
}

std::vector<pid_t> Process::process_list()
{
    std::vector<pid_t> vec;

    for (size_t i { 0 }; i < m_processes.size(); ++i)
    {
        if (by_pid(i))
        {
            vec.emplace_back(i);
        }
    }

    return vec;
}

Process *Process::create()
{
    pid_t free_idx = find_free_pid();
    if (free_idx == (int)m_processes.size())
    { // expand the process list
        m_processes.emplace_back(new Process);
    }
    else
    { // reuse a hole in the list
        m_processes[free_idx].reset(new Process);
    }

    if (m_processes[free_idx] == nullptr) return nullptr; // allocation failed ?

    m_processes[free_idx]->tgid = free_idx;
    m_processes[free_idx]->pid  = free_idx;

    ++m_process_count;

    kmsgbus.send(ProcessCreatedEvent{free_idx});

    return m_processes[free_idx].get();
}

Process *Process::create_init_task(void (*procedure)())
{
    assert(m_current_process == nullptr);
    m_current_process = create_kernel_task(procedure);

    Process::switch_mappings(nullptr, m_current_process);

    return m_current_process;
}

bool Process::check_args_size(const std::vector<kpp::string> &args)
{
    size_t tb_size { args.size()*sizeof(uintptr_t) };

    for (const auto& str : args)
    {
        tb_size += str.size()+1; // null terminator
    }

    return tb_size < Memory::page_size();
}

void Process::push_args(const std::vector<kpp::string>& args)
{
    data->args = args;

    // Structure:
    // 0..n*4 : ptr array
    // n*4..end : actual strings

    size_t table_size { args.size()*sizeof(uintptr_t) };

    for (const auto& str : args)
    {
        table_size += str.size()+1; // null terminator
    }

    expand_stack(table_size);

    Process::switch_mappings(&Process::current(), this);

    const uintptr_t argv_base = stack_pointer();
    const uintptr_t stack_base = argv_base;

    size_t cursor = args.size() * sizeof(uintptr_t); // start after arg array;
    assert(cursor < Memory::page_size());

    for (size_t i { 0 }; i < args.size(); ++i)
    {
        assert(cursor + args[i].size() < Memory::page_size());

        std::copy(args[i].c_str(), args[i].c_str() + args[i].size() + 1, (uint8_t*)stack_base + cursor); // include null terminator

        ((uint32_t*)stack_base)[i] = argv_base + cursor;

        cursor += args[i].size() + 1; // again, null terminator
    }

    push_onto_stack(argv_base); // argv
    push_onto_stack(args.size()); // argc

    Process::switch_mappings(this, &Process::current());
}

Process::~Process()
{
    // TODO : do this per fd ? and use destructor for this
    if (data->user_callbacks.use_count() == 1)
    {
        for (auto entry : data->user_callbacks->pages)
        {
            detach_fault_handler(entry.base);
        }
    }

    if (data->fd_table.use_count() == 1)
    {
        for (size_t fd { 0 }; fd < data->fd_table->size(); ++fd)
        {
            if (get_fd(fd))
            {
                close_fd(fd);
            }
        }
    }

    cleanup();
}
