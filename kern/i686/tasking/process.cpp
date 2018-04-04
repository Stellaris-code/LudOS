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

#include "utils/membuffer.hpp"
#include "utils/memutils.hpp"
#include "utils/align.hpp"

extern "C" void enter_ring3(const registers* regs);

// Let the upper page free for argv/argc
constexpr size_t user_stack_top = KERNEL_VIRTUAL_BASE - (1*Paging::page_size) - sizeof(uintptr_t);
constexpr uintptr_t argv_virt_page = KERNEL_VIRTUAL_BASE - (1*Paging::page_size);

std::unordered_map<uintptr_t, Process::AllocatedPageEntry>
copy_allocated_pages(const Process& source)
{
    std::unordered_map<uintptr_t, Process::AllocatedPageEntry> new_map;
    for (const auto& pair : source.allocated_pages)
    {
        new_map[pair.first].paddr = PhysPageAllocator::alloc_physical_page();
        new_map[pair.first].flags = pair.second.flags;

        auto src_ptr = Memory::mmap((void*)pair.second.paddr, Paging::page_size);
        auto dest_ptr = Memory::mmap((void*)new_map[pair.first].paddr, Paging::page_size);

        memcpy(dest_ptr, src_ptr, Paging::page_size);

        Memory::unmap(src_ptr, Paging::page_size);
        Memory::unmap(dest_ptr, Paging::page_size);
    }

    assert(new_map.size() == source.allocated_pages.size());
    return new_map;
}

uintptr_t copy_argv_page(const Process& source)
{
    uintptr_t new_page = PhysPageAllocator::alloc_physical_page();

    auto src_ptr = Memory::mmap((void*)source.arch_data->argv_phys_page, Paging::page_size);
    auto dest_ptr = Memory::mmap((void*)new_page, Paging::page_size);

    memcpy(dest_ptr, src_ptr, Paging::page_size);

    Memory::unmap(src_ptr, Paging::page_size);
    Memory::unmap(dest_ptr, Paging::page_size);

    return new_page;
}

void map_code(const Process::ArchSpecificData& data)
{
    size_t code_page_amnt = data.code.size() / Paging::page_size +
            (data.code.size()%Paging::page_size?1:0);

    for (size_t i { 0 }; i < code_page_amnt; ++i)
    {
        uint8_t* phys_addr = (uint8_t*)Paging::physical_address((uint8_t*)data.code.data() + i*Paging::page_size);
        uint8_t* virt_addr = (uint8_t*)(i * Paging::page_size);

        assert(phys_addr);
        Paging::map_page(phys_addr, virt_addr, Memory::Read|Memory::Write|Memory::User);
    }
}

void map_stack(const Process::ArchSpecificData& data)
{
    size_t code_page_amnt = data.stack.size() / Paging::page_size +
            (data.stack.size()%Paging::page_size?1:0);

    for (size_t i { 0 }; i < code_page_amnt; ++i)
    {
        uint8_t* phys_addr = (uint8_t*)Paging::physical_address((uint8_t*)data.stack.data() + i*Paging::page_size);
        uint8_t* virt_addr = (uint8_t*)(user_stack_top - (i * Paging::page_size));

        assert(phys_addr);
        Paging::map_page(phys_addr, virt_addr, Memory::Read|Memory::Write|Memory::NoExec|Memory::User);
    }
}

void map_allocated_pages(Process& p)
{
    for (const auto& pair : p.allocated_pages)
    {
        Paging::map_page((void*)pair.second.paddr, (void*)pair.first, pair.second.flags);
        //log_serial("Mapped 0x%x to 0x%x (PID %d)\n", pair.first, pair.second.paddr, p.pid);
    }
}

void unmap_allocated_pages(Process& p)
{
    for (const auto& pair : p.allocated_pages)
    {
        Paging::unmap_page((void*)pair.first);
        //log_serial("Unmapped 0x%x\n", pair.first);
    }
}

void unmap_code(const Process::ArchSpecificData& data)
{
    size_t code_page_amnt = data.code.size() / Paging::page_size +
            (data.code.size()%Paging::page_size?1:0);

    for (size_t i { 0 }; i < code_page_amnt; ++i)
    {
        uint8_t* virt_addr = (uint8_t*)(i * Paging::page_size);

        Paging::unmap_page(virt_addr);
    }
}

void unmap_stack(const Process::ArchSpecificData& data)
{
    size_t code_page_amnt = data.stack.size() / Paging::page_size +
            (data.stack.size()%Paging::page_size?1:0);

    for (size_t i { 0 }; i < code_page_amnt; ++i)
    {
        uint8_t* virt_addr = (uint8_t*)(user_stack_top - (i * Paging::page_size));

        Paging::unmap_page(virt_addr);
    }
}

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

        ((uint32_t*)data.data())[i] = addr + cursor;

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

void Process::set_args(gsl::span<const std::string> args)
{
    this->args.resize(args.size());
    std::copy(args.begin(), args.end(), this->args.begin());

    arch_data->args_need_update = true;
    arch_data->regs.eax = args.size();
}

void Process::wake_up(pid_t child, int err_code)
{
    arch_data->waitpid_return = err_code;
    arch_data->waitpid_return_update = true;

    waiting_pid.reset();

    arch_data->regs.eax = child; // set waitpid return value
}

void Process::arch_init(gsl::span<const uint8_t> code_to_copy, size_t allocated_size)
{
    if (arch_data) delete arch_data;
    arch_data = new ArchSpecificData;

    arch_data->stack.clear();
    arch_data->stack.resize(Paging::page_size);

    arch_data->code.clear();
    arch_data->code.resize(std::max<int>(code_to_copy.size(), allocated_size));
    std::copy(code_to_copy.begin(), code_to_copy.end(), arch_data->code.begin());

    arch_data->argv_phys_page = PhysPageAllocator::alloc_physical_page();

    set_args(args);

    registers regs;
    memset(&regs, 0, sizeof(registers));

    regs.eax = args.size();
    regs.ecx = argv_virt_page;
    regs.eip = current_pc;
    regs.esp = user_stack_top;
    regs.cs = gdt::user_code_selector*0x8 | 0x3;
    regs.ds = regs.es = regs.fs = regs.gs = regs.ss = gdt::user_data_selector*0x8 | 0x3;

    arch_data->regs = regs;
}

void Process::switch_to()
{
    {
        assert(!is_waiting());

        map_code(*arch_data);
        map_stack(*arch_data);

        Paging::map_page((void*)arch_data->argv_phys_page, (void*)argv_virt_page, Memory::Read|Memory::Write|Memory::User);

        map_allocated_pages(*this);

        if (arch_data->args_need_update)
        {
            populate_argv(argv_virt_page, this->args);
            arch_data->args_need_update = false;
        }

        if (arch_data->waitpid_return_update)
        {
            *wstatus = arch_data->waitpid_return;
            arch_data->waitpid_return_update = false;
        }

        m_current_process = this;
    }

    ALIGN_STACK(16);

    arch_data->regs.eip = current_pc;
    enter_ring3(&arch_data->regs);
}

void Process::unswitch()
{
    unmap_code(*arch_data);
    unmap_stack(*arch_data);

    Paging::unmap_page((void*)argv_virt_page);

    unmap_allocated_pages(*this);

    current_pc = arch_data->regs.eip;
}

void Process::cleanup()
{
    PhysPageAllocator::release_physical_page(arch_data->argv_phys_page);

    release_all_pages();
}

Process *Process::clone(Process &proc)
{
    auto new_proc = Process::create(proc.args);
    if (!new_proc) return nullptr;

    new_proc->name = proc.name;
    new_proc->uid = proc.uid;
    new_proc->gid = proc.gid;
    new_proc->parent = proc.pid;
    new_proc->pwd = proc.pwd;
    new_proc->fd_table = proc.fd_table;
    new_proc->allocated_pages = copy_allocated_pages(proc);
    new_proc->args = proc.args;
    new_proc->current_pc = proc.arch_data->regs.eip;
    new_proc->arch_data = new ArchSpecificData;
    *new_proc->arch_data = *proc.arch_data;
    new_proc->arch_data->argv_phys_page = copy_argv_page(proc);
    new_proc->arch_data->args_need_update = false;

    proc.children.emplace_back(new_proc->pid);

    assert(new_proc);
    return new_proc;
}

uintptr_t Process::allocate_pages(size_t pages)
{
    uint8_t* addr = reinterpret_cast<uint8_t*>(Paging::alloc_virtual_page(pages, true));
    for (size_t i { 0 }; i < pages; ++i)
    {
        void* virtual_page  = (uint8_t*)addr + i*Paging::page_size;
        void* physical_page = (void*)PhysPageAllocator::alloc_physical_page();
        Paging::map_page(physical_page,
                         virtual_page, Memory::Read|Memory::Write|Memory::User);

        assert(!allocated_pages.count((uintptr_t)virtual_page));
        allocated_pages[(uintptr_t)virtual_page] = {(uintptr_t)physical_page, Memory::Read|Memory::Write|Memory::User};
    }

    return (uintptr_t)addr;
}

bool Process::release_pages(uintptr_t ptr, size_t pages)
{
    assert(ptr % Paging::page_size == 0);

    for (size_t i { 0 }; i < pages; ++i)
    {
        void* virtual_page  = (uint8_t*)ptr + i*Paging::page_size;
        assert(allocated_pages.count((uintptr_t)virtual_page));

        void* physical_page = (void*)allocated_pages.at((uintptr_t)virtual_page).paddr;

        PhysPageAllocator::release_physical_page((uintptr_t)physical_page);
        if (Memory::is_mapped(virtual_page)) Paging::unmap_page(virtual_page);

        allocated_pages.erase((uintptr_t)virtual_page);
    }

    return true;
}
