/*
process_data.hpp

Copyright (c) 07 Yann BOUCHER (yann)

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
#ifndef PROCESS_DATA_HPP
#define PROCESS_DATA_HPP

#include <stdint.h>
#include <string.hpp>
#include <memory.hpp>
#include <optional.hpp>
#include <vector.hpp>
#include <unordered_map.hpp>

#include <sys/types.h>

#include "shared_memory.hpp"

namespace vfs
{
class node;
}

namespace tasking
{
// TODO : merge the two
struct AllocatedPageEntry
{
    uintptr_t paddr;
    uint32_t  flags;
};
struct MemoryMapping
{
    void*     vaddr;
    uintptr_t paddr;
    uint32_t  flags;
};

struct ShmEntry
{
    std::shared_ptr<SharedMemorySegment> shm;
    void* v_addr;
};
struct FDInfo
{
    std::shared_ptr<vfs::node> node;
    bool read { false };
    bool write { false };
    bool append { false };
    size_t cursor { 0 };
};
}

struct ProcessData
{
    std::string name { "<INVALID>" };
    uint32_t uid { 0 };
    uint32_t gid { 0 };

    std::vector<pid_t> children;

    std::optional<pid_t> waiting_pid;
    int* wstatus { nullptr };
    uintptr_t waitstatus_phys { 0 };

    std::string pwd = "/";

    std::vector<tasking::FDInfo> fd_table;

    std::vector<tasking::MemoryMapping> mappings;
    std::unordered_map<uintptr_t, tasking::AllocatedPageEntry> allocated_pages;

    std::vector<std::string> args;
    uintptr_t argv_phys_page;

    std::unordered_map<unsigned int, tasking::ShmEntry> shm_list;

    uintptr_t current_pc { 0 };
};

#endif // PROCESS_DATA_HPP
