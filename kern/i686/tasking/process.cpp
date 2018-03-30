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

#include "i686/gdt/gdt.hpp"
#include "i686/interrupts/interrupts.hpp"
#include "i686/mem/physallocator.hpp"

#include "utils/membuffer.hpp"
#include "utils/memutils.hpp"
#include "utils/align.hpp"

extern "C" void enter_ring3(const registers* regs);

constexpr size_t user_stack_top = KERNEL_VIRTUAL_BASE - sizeof(uintptr_t);

bool args_need_update { false };

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

bool Process::check_args_size(gsl::span<const std::string> args)
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

    args_need_update = true;
}

void Process::execute()
{
    {
        map_code(*arch_data);
        map_stack(*arch_data);

        arch_data->argv_page = Paging::alloc_virtual_page(1, true);
        Paging::map_page((void*)PhysPageAllocator::alloc_physical_page(), (void*)arch_data->argv_page, Memory::Read|Memory::Write|Memory::User);

        if (args_need_update)
        {
            populate_argv(arch_data->argv_page, this->args);
            args_need_update = false;
        }

        m_current_process = this;
    }

    ALIGN_STACK(16);

    arch_data->regs.eip = start_address;

    enter_ring3(&arch_data->regs);
}

Process *Process::clone(Process &proc)
{
    auto new_proc = Process::create(proc.args);
    if (!new_proc) return nullptr;

    new_proc->name = proc.name;
    new_proc->uid = proc.uid;
    new_proc->gid = proc.gid;
    new_proc->pwd = proc.pwd;
    new_proc->fd_table = proc.fd_table;
    new_proc->allocated_pages = proc.allocated_pages;
    new_proc->start_address = proc.arch_data->regs.eip;
    new_proc->arch_data = new ArchSpecificData;
    *new_proc->arch_data = *proc.arch_data;

    assert(new_proc);
    return new_proc;
}

void Process::stop()
{
    unmap_code(*arch_data);
    unmap_stack(*arch_data);

    PhysPageAllocator::release_physical_page(Paging::physical_address((void*)arch_data->argv_page));
    Paging::unmap_page((void*)arch_data->argv_page);

    release_allocated_pages();

    start_address = arch_data->regs.eip;
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

    set_args(args);

    registers regs;
    memset(&regs, 0, sizeof(registers));

    regs.eax = args.size();
    regs.ecx = arch_data->argv_page;
    regs.eip = start_address;
    regs.esp = user_stack_top;
    regs.cs = gdt::user_code_selector*0x8 | 0x3;
    regs.ds = regs.es = regs.fs = regs.gs = regs.ss = gdt::user_data_selector*0x8 | 0x3;

    arch_data->regs = regs;
}
