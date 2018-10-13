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

void Process::map_shm()
{
    for (auto pair : data->shm_list)
    {
        if (pair.second.v_addr) pair.second.shm->map(pair.second.v_addr);
    }
}

void Process::map_code()
{
    size_t code_page_amnt = data->code->size() / Memory::page_size() +
            (data->code->size()%Memory::page_size()?1:0);

    for (size_t i { 0 }; i < code_page_amnt; ++i)
    {
        uintptr_t phys_addr = Memory::physical_address((uint8_t*)data->code->data() + i*Memory::page_size());
        uint8_t* virt_addr = (uint8_t*)(i * Memory::page_size());

        assert(phys_addr);
        data->mappings[(uintptr_t)virt_addr] = {phys_addr, Memory::Read|Memory::Write|Memory::Executable|Memory::User, false};
    }
}

void Process::map_stack()
{
    size_t stack_page_amnt = data->stack.size() / Memory::page_size() +
            (data->stack.size()%Memory::page_size()?1:0);

    for (size_t i { 0 }; i <stack_page_amnt; ++i)
    {
        uintptr_t phys_addr = Memory::physical_address((uint8_t*)data->stack.data()
                                                       + (stack_page_amnt-i-1)*Memory::page_size());
        uint8_t* virt_addr = (uint8_t*)Memory::page(user_stack_top-Memory::page_size()) - i*Memory::page_size();

        assert(phys_addr);
        data->mappings[(uintptr_t)virt_addr] = {phys_addr, Memory::Read|Memory::Write|Memory::User, false};
    }
}

void Process::create_mappings()
{
    map_code();
    map_stack();
    if (!data->mappings.count(argv_virt_page))
    {
        data->mappings[argv_virt_page] =
        {Memory::allocate_physical_page(), Memory::Read|Memory::Write|Memory::User, true};
    }
    if (!data->mappings.count(signal_trampoline_page))
    {
        data->mappings[signal_trampoline_page] =
        {Memory::physical_address((void*)signal_trampoline), Memory::Read|Memory::User, false};
    }
}

void Process::release_mappings()
{
    for (const auto& pair : data->mappings)
    {
        if (pair.second.owned)
        {
            Memory::release_physical_page(pair.second.paddr);
        }
    }

    data->mappings.clear();
}

void Process::map_address_space()
{
    for (const auto& pair : data->mappings)
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
        data->mappings[virt + Memory::page_size()*i] = {phys + Memory::page_size()*i,
                Memory::Read|Memory::Write|Memory::WriteThrough|Memory::User, false};
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
    auto virt_page = Memory::allocate_virtual_page(1, true);
    Memory::map_page(0, (void*)virt_page, Memory::Sentinel|Memory::User);
    data->mappings[(uintptr_t)virt_page] = {0, Memory::Sentinel|Memory::User, false};

    // Mark these new callbacks as free
    for (size_t i { 0 }; i < Memory::page_size(); ++i)
    {
        data->user_callbacks->free_entries.insert(virt_page+i);
    }

    auto handle = attach_fault_handler((void*)virt_page, [this](const PageFault& fault)
    {
        if (fault.level != PageFault::User) return false;
        if (fault.type != PageFault::Read && fault.type != PageFault::Execute) return false;
        if (!data->user_callbacks->list.count(fault.address)) return false;

        auto entry = data->user_callbacks->list.at(fault.address);
        do_user_callback(entry.callback, entry.arg_sizes);

        return true;
    });

    data->user_callbacks->pages.push_back(UserCallbacks::PageEntry{virt_page, handle});
}

uintptr_t Process::allocate_pages(size_t pages)
{
    uint8_t* addr = reinterpret_cast<uint8_t*>(Memory::allocate_virtual_page(pages, true));
    for (size_t i { 0 }; i < pages; ++i)
    {
        void* virtual_page  = (uint8_t*)addr + i*Memory::page_size();
        uintptr_t physical_page = Memory::allocate_physical_page();
        Memory::map_page(physical_page, virtual_page, Memory::Read|Memory::Write|Memory::User);

        assert(!data->mappings.count((uintptr_t)virtual_page));
        data->mappings[(uintptr_t)virtual_page] = {(uintptr_t)physical_page, Memory::Read|Memory::Write|Memory::User, true};
    }

    return (uintptr_t)addr;
}

bool Process::release_pages(uintptr_t ptr, size_t pages)
{
    assert(ptr % Memory::page_size() == 0);

    for (size_t i { 0 }; i < pages; ++i)
    {
        void* virtual_page  = (uint8_t*)ptr + i*Memory::page_size();
        assert(data->mappings.count((uintptr_t)virtual_page));

        uintptr_t physical_page = data->mappings.at((uintptr_t)virtual_page).paddr;

        Memory::release_physical_page(physical_page);
        if (Memory::is_mapped(virtual_page)) Memory::unmap_page(virtual_page);

        assert(data->mappings.at((uintptr_t)virtual_page).owned);
        data->mappings.erase((uintptr_t)virtual_page);
    }

    return true;
}

void Process::copy_allocated_pages(Process &target)
{
    for (const auto& pair : data->mappings)
    {
        if (!pair.second.owned)
            continue;

        target.data->mappings[pair.first] = pair.second;
        assert(target.data->mappings[pair.first].owned);
        target.data->mappings[pair.first].paddr = Memory::allocate_physical_page();

        auto src_ptr = Memory::mmap(pair.second.paddr, Memory::page_size());
        auto dest_ptr = Memory::mmap(target.data->mappings[pair.first].paddr, Memory::page_size());

        aligned_memcpy(dest_ptr, src_ptr, Memory::page_size());

        Memory::unmap(src_ptr, Memory::page_size());
        Memory::unmap(dest_ptr, Memory::page_size());
    }
}
