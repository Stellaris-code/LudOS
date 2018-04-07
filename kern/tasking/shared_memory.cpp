/*
shared_memory.cpp

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

#include "shared_memory.hpp"

#include <unordered_map.hpp>

#include <sys/ipc.h>

#include "mem/memmap.hpp"
#include "utils/stlutils.hpp"
#include "utils/logging.hpp"

SharedMemorySegment::SharedMemorySegment(size_t size_in_pages)
{
    for (size_t i { 0 }; i < size_in_pages; ++i)
    {
        m_phys_addrs.emplace_back(Memory::allocate_physical_page());
    }

    log_serial("SHM creation : 0x%x\n", m_phys_addrs[0]);
}

SharedMemorySegment::~SharedMemorySegment()
{
    log_serial("SHM destruction : 0x%x\n", m_phys_addrs[0]);

    for (auto addr : m_phys_addrs)
    {
        Memory::release_physical_page(addr);
    }

    m_phys_addrs.clear();
}

void SharedMemorySegment::map(void *v_addr, uint32_t flags)
{
    for (size_t i { 0 }; i < m_phys_addrs.size(); ++i)
    {
        Memory::map_page(m_phys_addrs[i], (uint8_t*)v_addr + i*Memory::page_size(), flags);
    }
}

void SharedMemorySegment::unmap(void *v_addr)
{
    for (size_t i { 0 }; i < m_phys_addrs.size(); ++i)
    {
        Memory::unmap_page((uint8_t*)v_addr + i*Memory::page_size());
    }
}

size_t SharedMemorySegment::size() const
{
    return m_phys_addrs.size() * Memory::page_size();
}

static std::unordered_map<unsigned int, std::weak_ptr<SharedMemorySegment>> shmlist;

std::shared_ptr<SharedMemorySegment> create_shared_mem(unsigned int id, size_t size)
{
    assert(!shmlist.count(id));

    auto ptr = std::make_shared<SharedMemorySegment>(size);
    if (ptr) shmlist[id] = ptr;
    return ptr;
}

std::shared_ptr<SharedMemorySegment> get_shared_mem(unsigned int id)
{
    if (shmlist.find(id) == shmlist.end() || shmlist.at(id).expired())
    {
        return nullptr;
    }

    return shmlist[id].lock();
}

unsigned int create_shared_memory_id()
{
    unsigned int id = 0;
    for (const auto& pair : shmlist)
    {
        if (id <= pair.first) id = pair.first + 1;
    }

    return id;
}
