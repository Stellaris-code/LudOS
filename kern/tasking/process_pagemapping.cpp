/*
process_pagemapping.cpp

Copyright (c) 13 Yann BOUCHER (yann)

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
#include "tasking/process_data.hpp"

extern "C" void signal_trampoline();

using namespace tasking;

void Process::unmap_address_space()
{
    for (const auto& pair : *data->mappings)
    {
        Memory::unmap_page((void*)pair.first);
    }
}

void Process::map_code(gsl::span<const uint8_t> code, size_t allocated_size)
{
    size_t code_page_amnt = allocated_size / Memory::page_size() +
            (allocated_size%Memory::page_size()?1:0);

    for (size_t i { 0 }; i < code_page_amnt; ++i)
    {
        uintptr_t phys_addr = Memory::allocate_physical_page();
        uint8_t* virt_addr = (uint8_t*)(i * Memory::page_size()) + USER_VIRTUAL_BASE;

        // allow allocation of .bss physical pages while preventing copying data outside of "code"
        const int copy_size = std::min<int>((int)code.size() - i*Memory::page_size(), Memory::page_size());
        auto src_ptr = code.data() + i*Memory::page_size();
        auto dest_ptr = Memory::mmap(phys_addr, Memory::page_size());

        if (copy_size > 0)
        {
            size_t page_bss_size = Memory::page_size() - copy_size;

            memcpy(dest_ptr, src_ptr, copy_size);

            memset((uint8_t*)dest_ptr + copy_size, 0, page_bss_size);
        }
        else
        {
            aligned_memsetl(dest_ptr, 0, Memory::page_size());
        }

        Memory::unmap(dest_ptr, Memory::page_size());

        assert(phys_addr);
        map_page((uintptr_t)virt_addr, phys_addr, Memory::Read|Memory::Write|Memory::Executable|Memory::User, true);
    }
}

void Process::map_stack(size_t stack_size)
{
    size_t stack_page_amnt = stack_size / Memory::page_size() +
            (stack_size%Memory::page_size()?1:0);

    for (size_t i { 0 }; i <stack_page_amnt; ++i)
    {
        uintptr_t phys_addr = Memory::allocate_physical_page();
        uint8_t* virt_addr = (uint8_t*)Memory::page(user_stack_top-Memory::page_size()) - i*Memory::page_size();

        assert(phys_addr);
        map_page((uintptr_t)virt_addr, phys_addr, Memory::Read|Memory::Write|Memory::User, true);
    }
}

void Process::create_mappings(gsl::span<const uint8_t> code, size_t allocated_size, size_t stack_size)
{
    map_code(code, allocated_size);
    map_stack(stack_size);

    map_page(signal_trampoline_page, Memory::physical_address((void*)signal_trampoline), Memory::Read|Memory::User|Memory::Executable, false);
}

void Process::release_mappings()
{
    // don't delete physical pages if we share the address space with another process
    if (data->mappings.use_count() > 1)
        return;

    for (const auto& pair : *data->mappings)
    {
        if (pair.second.owned)
        {
            Memory::release_physical_page(pair.second.paddr);
        }
    }

    data->mappings->clear();
}

uintptr_t Process::allocate_virtual_page(size_t count)
{
    uintptr_t addr = 0;
    size_t counter = 0;
    for (size_t i { USER_VIRTUAL_BASE >> 12 }; i < KERNEL_VIRTUAL_BASE >> 12; ++i)
    {
        if (!data->mappings->count(i << 12))
        {
            if (counter++ == 0) addr = i << 12;
        }
        else
        {
            counter = 0;
        }

        if (counter == count)
        {
            return addr;
        }
    }

    assert(false);
}

void Process::map_page(uintptr_t virt_addr, uintptr_t phys_addr, uint32_t flags, bool owned)
{
    if (data->mappings == m_current_process->data->mappings)
        Memory::map_page(phys_addr, (void*)virt_addr, flags);

    assert(!data->mappings->count((uintptr_t)virt_addr));
    (*data->mappings)[(uintptr_t)virt_addr] = {phys_addr, flags, owned};
}

void Process::map_address_space()
{
    for (const auto& pair : *data->mappings)
    {
        Memory::map_page(pair.second.paddr, (void*)pair.first, pair.second.flags);
    }
}

void *Process::map_range(uintptr_t phys, size_t len)
{
    size_t page_count = len / Memory::page_size() + (len%Memory::page_size()?1:0);
    uintptr_t virt = Memory::allocate_virtual_page(page_count, true);
    for (size_t i { 0 }; i < page_count; ++i)
    {
        map_page(virt + Memory::page_size()*i, phys + Memory::page_size()*i, Memory::Read|Memory::Write|Memory::User, false);
    }

    return (void*)virt;
}

uintptr_t Process::create_user_callback_impl(const std::function<int(const std::vector<uintptr_t>&)> &callback, const std::vector<size_t> &arg_sizes)
{
    if (data->user_callbacks->free_entries.empty())
    {
        allocate_user_callback_page();
    }
    assert(!data->user_callbacks->free_entries.empty());

    const auto address = *data->user_callbacks->free_entries.begin();
    data->user_callbacks->list[address] = {arg_sizes, callback};
    data->user_callbacks->free_entries.erase(data->user_callbacks->free_entries.begin());

    return address;
}

void Process::allocate_user_callback_page()
{
    auto virt_page = allocate_virtual_page(1);
    map_page((uintptr_t)virt_page, 0, Memory::Sentinel|Memory::User, false);


    // Mark these new callbacks as free
    for (size_t i { 0 }; i < Memory::page_size(); ++i)
    {
        data->user_callbacks->free_entries.insert(virt_page+i);
    }

    auto handle = attach_fault_handler((void*)virt_page, [this](const PageFault& fault)
    {
        return user_callback_fault_handler(fault);
    });

    data->user_callbacks->pages.push_back(UserCallbacks::PageEntry{virt_page, handle});
}

uintptr_t Process::allocate_pages(size_t pages)
{
    // TODO : use vmalloc
    uint8_t* addr = (uint8_t*)allocate_virtual_page(pages);
    for (size_t i { 0 }; i < pages; ++i)
    {
        void* virtual_page  = (uint8_t*)addr + i*Memory::page_size();
        uintptr_t physical_page = Memory::allocate_physical_page();

        map_page((uintptr_t)virtual_page, (uintptr_t)physical_page, Memory::Read|Memory::Write|Memory::User, true);
    }

    return (uintptr_t)addr;
}

bool Process::release_pages(uintptr_t ptr, size_t pages)
{    
    // TODO : use vfree
    assert(ptr % Memory::page_size() == 0);

    for (size_t i { 0 }; i < pages; ++i)
    {
        void* virtual_page  = (uint8_t*)ptr + i*Memory::page_size();
        assert(data->mappings->count((uintptr_t)virtual_page));

        uintptr_t physical_page = data->mappings->at((uintptr_t)virtual_page).paddr;

        Memory::release_physical_page(physical_page);
        Memory::unmap_page(virtual_page);

        assert(data->mappings->at((uintptr_t)virtual_page).owned);
        data->mappings->erase((uintptr_t)virtual_page);
    }

    return true;
}

void Process::copy_page_directory(Process &target)
{
    for (const auto& pair : *data->mappings)
    {
        if (!pair.second.owned)
            continue;

        (*target.data->mappings)[pair.first] = pair.second;
        assert((*target.data->mappings)[pair.first].owned);
        (*target.data->mappings)[pair.first].paddr = Memory::allocate_physical_page();

        auto src_ptr = Memory::mmap(pair.second.paddr, Memory::page_size());
        auto dest_ptr = Memory::mmap((*target.data->mappings)[pair.first].paddr, Memory::page_size());

        aligned_memcpy(dest_ptr, src_ptr, Memory::page_size());

        Memory::unmap(src_ptr, Memory::page_size());
        Memory::unmap(dest_ptr, Memory::page_size());
    }
}

void Process::init_tls()
{
    // top of the tls area
    uint8_t* addr = (uint8_t*)allocate_virtual_page(tls_pages + 1);

    uintptr_t tls_control_vaddr;
    uintptr_t tls_control_paddr;

    for (size_t i { 0 }; i < tls_pages + 1; ++i)
    {
        void* virtual_page  = (uint8_t*)addr + i*Memory::page_size();
        uintptr_t physical_page = Memory::allocate_physical_page();

        map_page((uintptr_t)virtual_page, (uintptr_t)physical_page, Memory::Read|Memory::Write|Memory::User, true);

        if (i == tls_pages)
        {
            tls_control_vaddr = (uintptr_t)virtual_page;
            tls_control_paddr = (uintptr_t)physical_page;
        }
    }

    // set the first TLS entry to the address of the TLS section
    Memory::phys_write(tls_control_paddr, &tls_control_vaddr, 4);

    data->tls_vaddr = tls_control_vaddr;
}

void Process::switch_mappings(Process* prev, Process *next)
{
    if (prev == nullptr)
    {
        next->map_address_space();
        next->switch_tls();

        return;
    }

    // only switch address spaces if they aren't shared
    if (prev->data->mappings != next->data->mappings)
    {
        prev->unmap_address_space();
        next->map_address_space();
    }

    next->switch_tls();
}
