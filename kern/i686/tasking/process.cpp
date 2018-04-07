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

#include "mem/meminfo.hpp"

extern "C" void enter_ring3(const registers* regs);

// Let the upper page free for argv/argc
constexpr size_t user_stack_top = KERNEL_VIRTUAL_BASE - (1*Paging::page_size) - sizeof(uintptr_t);
constexpr uintptr_t argv_virt_page = KERNEL_VIRTUAL_BASE - (1*Paging::page_size);

void map_code(Process& p)
{
    size_t code_page_amnt = p.arch_data->code.size() / Paging::page_size +
            (p.arch_data->code.size()%Paging::page_size?1:0);

    for (size_t i { 0 }; i < code_page_amnt; ++i)
    {
        uintptr_t phys_addr = Paging::physical_address((uint8_t*)p.arch_data->code.data() + i*Paging::page_size);
        uint8_t* virt_addr = (uint8_t*)(i * Paging::page_size);

        assert(phys_addr);
        p.data.mappings.push_back({virt_addr, phys_addr, VM::Read|VM::Write|VM::User});
    }
}

void map_stack(Process& p)
{
    size_t code_page_amnt = p.arch_data->stack.size() / Paging::page_size +
            (p.arch_data->stack.size()%Paging::page_size?1:0);

    for (size_t i { 0 }; i < code_page_amnt; ++i)
    {
        uintptr_t phys_addr = Paging::physical_address((uint8_t*)p.arch_data->stack.data() + i*Paging::page_size);
        uint8_t* virt_addr = (uint8_t*)(user_stack_top - (i * Paging::page_size));

        assert(phys_addr);
        p.data.mappings.push_back({virt_addr, phys_addr, VM::Read|VM::Write|VM::User});
    }
}

void map_allocated_pages(Process& p)
{
    for (const auto& pair : p.data.allocated_pages)
    {
        Paging::map_page(pair.second.paddr, (void*)pair.first, pair.second.flags);
    }
}

void Process::create_mappings()
{
    map_code(*this);
    map_stack(*this);
    //map_allocated_pages(*this);
    data.mappings.push_back({(void*)argv_virt_page, data.argv_phys_page, VM::Read|VM::Write|VM::User});
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

    auto ptr = VM::mmap(data.argv_phys_page, Paging::page_size);
    populate_argv((uintptr_t)ptr, args);
    VM::unmap(ptr, Paging::page_size);

    arch_data->regs.eax = args.size();
}

void Process::wake_up(pid_t child, int err_code)
{
    VM::phys_write(data.waitstatus_phys, &err_code, sizeof(err_code));

    data.waiting_pid.reset();

    arch_data->regs.eax = child; // set waitpid return value
}

void Process::arch_init(gsl::span<const uint8_t> code_to_copy, size_t allocated_size)
{
    arch_data = new ArchSpecificData;

    arch_data->stack.resize(Paging::page_size);

    arch_data->code.resize(std::max<int>(code_to_copy.size(), allocated_size));
    std::copy(code_to_copy.begin(), code_to_copy.end(), arch_data->code.begin());

    data.argv_phys_page = PhysPageAllocator::alloc_physical_page();

    set_args(data.args);

    registers regs;
    memset(&regs, 0, sizeof(registers));

    regs.eax = data.args.size();
    regs.ecx = argv_virt_page;
    regs.eip = data.current_pc;
    regs.esp = user_stack_top;
    regs.cs = gdt::user_code_selector*0x8 | 0x3;
    regs.ds = regs.es = regs.fs = regs.gs = regs.ss = gdt::user_data_selector*0x8 | 0x3;

    arch_data->regs = regs;

    create_mappings();
}

void Process::switch_to()
{
    {
        assert(!is_waiting());

        map_address_space();
        map_allocated_pages(*this);
        map_shm();

        arch_data->regs.eip = data.current_pc;

        m_current_process = this;
    }
    enter_ring3(&arch_data->regs);
}

void Process::unswitch()
{
    unmap_address_space();

    data.current_pc = arch_data->regs.eip;
}

Process *Process::clone(Process &proc)
{
    auto new_proc = Process::create(proc.data.args);
    if (!new_proc) return nullptr;

    // TODO : move the copy constructor

    new_proc->data.name = proc.data.name;
    new_proc->data.uid = proc.data.uid;
    new_proc->data.gid = proc.data.gid;
    new_proc->parent = proc.pid;
    new_proc->data.pwd = proc.data.pwd;
    new_proc->data.fd_table = proc.data.fd_table;
    new_proc->data.allocated_pages = proc.copy_allocated_pages();
    new_proc->data.args = proc.data.args;
    new_proc->data.current_pc = proc.arch_data->regs.eip;
    new_proc->data.argv_phys_page = proc.copy_argv_page();
    new_proc->data.shm_list = proc.data.shm_list;
    new_proc->arch_data = new ArchSpecificData;
    *new_proc->arch_data = *proc.arch_data;

    proc.data.children.emplace_back(new_proc->pid);

    new_proc->create_mappings();

    assert(new_proc);
    return new_proc;
}

uintptr_t Process::allocate_pages(size_t pages)
{
    uint8_t* addr = reinterpret_cast<uint8_t*>(Paging::alloc_virtual_page(pages, true));
    for (size_t i { 0 }; i < pages; ++i)
    {
        // TODO : delete allocated_pages
        void* virtual_page  = (uint8_t*)addr + i*Paging::page_size;
        uintptr_t physical_page = PhysPageAllocator::alloc_physical_page();
        Paging::map_page(physical_page, virtual_page, VM::Read|VM::Write|VM::User);

        assert(!data.allocated_pages.count((uintptr_t)virtual_page));
        data.allocated_pages[(uintptr_t)virtual_page] = {(uintptr_t)physical_page, VM::Read|VM::Write|VM::User};
    }

    return (uintptr_t)addr;
}

bool Process::release_pages(uintptr_t ptr, size_t pages)
{
    assert(ptr % Paging::page_size == 0);

    for (size_t i { 0 }; i < pages; ++i)
    {
        void* virtual_page  = (uint8_t*)ptr + i*Paging::page_size;
        assert(data.allocated_pages.count((uintptr_t)virtual_page));

        void* physical_page = (void*)data.allocated_pages.at((uintptr_t)virtual_page).paddr;

        PhysPageAllocator::release_physical_page((uintptr_t)physical_page);
        if (VM::is_mapped(virtual_page)) Paging::unmap_page(virtual_page);

        data.allocated_pages.erase((uintptr_t)virtual_page);
    }

    return true;
}

void Process::unmap_address_space()
{
    Paging::unmap_user_space();
}

void Process::cleanup()
{
    VM::release_physical_page(data.argv_phys_page);

    release_all_pages();

    data.mappings.clear();

    delete arch_data;
    arch_data = nullptr;
}

Process::~Process()
{
    unswitch();
    cleanup();
    log_serial("PID destroyed : %d Rem %s\n", pid, human_readable_size(MemoryInfo::free()).c_str());
}
