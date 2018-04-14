/*
process.cpp

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

#include "tasking/process.hpp"
#include "i686/tasking/process.hpp"

#include <string.hpp>
#include <vector.hpp>

#include <sys/wait.h>

#include "i686/gdt/gdt.hpp"
#include "i686/interrupts/interrupts.hpp"
#include "i686/mem/physallocator.hpp"
#include "utils/aligned_vector.hpp"

#include "syscalls/syscall_list.hpp"

#include "fs/fsutils.hpp"
#include "fs/vfs.hpp"

#include "utils/membuffer.hpp"
#include "utils/align.hpp"

extern "C" [[noreturn]] void enter_ring3(const registers* regs);

// Let the upper page free for argv/argc
constexpr uintptr_t argv_virt_page = KERNEL_VIRTUAL_BASE - (1*Paging::page_size);

void populate_argv(uintptr_t addr, gsl::span<const std::string> args)
{
    // Structure:
    // 0..n*4 : ptr array
    // n*4..end : actual strings

    gsl::span<uint8_t> data {(uint8_t*)addr, Paging::page_size};

    size_t cursor = args.size() * sizeof(uintptr_t); // start after arg array;

    for (size_t i { 0 }; i < args.size(); ++i)
    {
        assert(cursor + args[i].size() < Paging::page_size);

        std::copy(args[i].c_str(), args[i].c_str() + args[i].size() + 1, data.data() + cursor); // include null terminator

        ((uint32_t*)data.data())[i] = argv_virt_page + cursor;

        cursor += args[i].size() + 1; // again, null terminator
    }

    assert(cursor < Paging::page_size);
}

bool Process::check_args_size(const std::vector<std::string> &args)
{
    size_t tb_size { args.size()*sizeof(uintptr_t) };

    for (const auto& str : args)
    {
        tb_size += str.size()+1; // null terminator
    }

    return tb_size < Paging::page_size;
}

void Process::set_args(const std::vector<std::string>& args)
{
    data.args = args;

    auto ptr = Memory::mmap(data.mappings.at(argv_virt_page).paddr, Paging::page_size);
    populate_argv((uintptr_t)ptr, args);
    Memory::unmap(ptr, Paging::page_size);

    arch_context->regs.eax = args.size();
}

void Process::wake_up(pid_t child, int err_code)
{
    Memory::phys_write(data.waitstatus_phys, &err_code, sizeof(err_code));

    data.waiting_pid.reset();

    arch_context->regs.eax = child; // set waitpid return value
}

void Process::arch_init(gsl::span<const uint8_t> code_to_copy, size_t allocated_size)
{
    assert(!arch_context);
    arch_context = new ArchContext;

    data.stack.resize(Paging::page_size * 2);

    data.code = std::make_shared<aligned_vector<uint8_t, Memory::page_size()>>();
    data.code->resize(std::max<int>(code_to_copy.size(), allocated_size));
    std::copy(code_to_copy.begin(), code_to_copy.end(), data.code->begin());
    std::fill(data.code->begin() + code_to_copy.size(), data.code->end(), 0); // clear the allocated part

    registers regs;
    memset(&regs, 0, sizeof(registers));

    regs.eax = data.args.size();
    regs.ecx = argv_virt_page;
    regs.eip = data.current_pc;
    regs.esp = user_stack_top;
    regs.cs = gdt::user_code_selector*0x8 | 0x3;
    regs.ds = regs.es = regs.fs = regs.gs = regs.ss = gdt::user_data_selector*0x8 | 0x3;

    arch_context->regs = regs;

    create_mappings();

    set_args(data.args);
}

void Process::switch_to()
{
    {
        assert(!is_waiting());

        map_address_space();
#ifdef LUDOS_HAS_SHM
        map_shm();
#endif

        arch_context->regs.eip = data.current_pc;

        m_current_process = this;
    }
    enter_ring3(&arch_context->regs);
}

void Process::unswitch()
{
    unmap_address_space();

    data.current_pc = arch_context->regs.eip;
}

Process *Process::clone(Process &proc, uint32_t flags)
{
    auto new_proc = Process::create(proc.data.args);
    if (!new_proc) return nullptr;

    new_proc->data.code = std::make_shared<aligned_vector<uint8_t, Memory::page_size()>>(*proc.data.code); // noleak ? :(
    new_proc->data.stack = proc.data.stack; // noleak
    new_proc->data.name = proc.data.name + "_child"; // noleak
    new_proc->data.uid = proc.data.uid;
    new_proc->data.gid = proc.data.gid;
    new_proc->parent = proc.pid;

    new_proc->data.pwd  = vfs::find(proc.data.pwd->path()); assert(new_proc->data.pwd);
    new_proc->data.root = vfs::find(proc.data.root->path()); assert(new_proc->data.root);

    new_proc->data.fd_table = std::make_shared<std::vector<tasking::FDInfo>>(*proc.data.fd_table); // noleak
    proc.copy_allocated_pages(*new_proc); // noleak
    new_proc->data.args = proc.data.args; // noleak
    new_proc->data.current_pc = proc.arch_context->regs.eip;
#ifdef LUDOS_HAS_SHM
    new_proc->data.shm_list = proc.data.shm_list;
#endif
    new_proc->arch_context = new ArchContext;
    *new_proc->arch_context = *proc.arch_context;

    proc.data.children.emplace_back(new_proc->pid);

    new_proc->create_mappings();

    assert(new_proc);
    return new_proc;
}

void Process::unmap_address_space()
{
#if 1
    Paging::unmap_user_space();
#else
    for (const auto& pair : data.mappings)
    {
        Memory::unmap_page((void*)pair.first);
    }
    for (const auto& shm : data.shm_list)
    {
        Memory::unmap(shm.second.v_addr, shm.second.shm->size());
    }
#endif
}

void Process::cleanup()
{
    release_mappings();

    delete arch_context;
    arch_context = nullptr;
}
