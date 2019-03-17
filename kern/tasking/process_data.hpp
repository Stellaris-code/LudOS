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

#include "config.hpp"

#include <stdint.h>
#include <memory.hpp>
#include <optional.hpp>
#include <vector.hpp>
#include <unordered_map.hpp>
#include <unordered_set.hpp>
#include <stack.hpp>

#include <kstring/kstring.hpp>

#include <sys/types.h>
#include <signal.h>

#include "mem/memmap.hpp"
#include "mem/page_fault.hpp"

#include "shared_memory.hpp"

#include "fdinfo.hpp"

#include "utils/aligned_vector.hpp"

namespace vfs
{
class node;
}

namespace tasking
{
struct MemoryMapping
{
    uintptr_t paddr;
    uint32_t  flags : 31;
    bool      owned : 1; // TODO : use an enum
};

struct ShmEntry
{
    std::shared_ptr<SharedMemorySegment> shm;
    void* v_addr;
};

struct UserCallbacks
{
    struct CallbackEntry
    {
        std::vector<size_t> arg_sizes;
        std::function<int(const std::vector<uintptr_t>&)> callback;
    };
    struct PageEntry
    {
        uintptr_t base;
        fault_handle page_fault_handle;
    };

    std::unordered_set<uintptr_t> free_entries;
    std::unordered_map<uintptr_t, CallbackEntry> list;
    std::vector<PageEntry> pages;
};
}

struct ProcessArchContext;

// Don't forget to update clone() when adding fields !
struct ProcessData
{
    static constexpr size_t kernel_stack_size = 0x4000;

    template <typename T>
    using shared_resource = std::shared_ptr<T>;

    aligned_vector<uint8_t, Memory::page_size()> stack; // stack is never shared
    uint8_t* kernel_stack { nullptr }; // TODO : have a class for that
    shared_resource<aligned_vector<uint8_t, Memory::page_size()>> code;

    kpp::string name { "<INVALID>" };
    uint32_t uid { 0 };
    uint32_t gid { 0 };

    std::vector<pid_t> children;

    kpp::optional<pid_t> waiting_pid;
    pid_t waitpid_child { 0 };
    uintptr_t waitstatus_phys { 0 };

    shared_resource<vfs::node> pwd;
    shared_resource<vfs::node> root;

    shared_resource<std::vector<tasking::FDInfo>> fd_table;

    shared_resource<tasking::UserCallbacks> user_callbacks;

    std::unordered_map<uintptr_t, tasking::MemoryMapping> mappings;

    std::vector<kpp::string> args;

    std::unordered_map<unsigned int, tasking::ShmEntry> shm_list;

    struct SigContext
    {
        ProcessArchContext* cpu_context;
        pid_t returning_process;
    };
    std::stack<SigContext> sig_context;
    shared_resource<kpp::array<struct sigaction, SIGRTMAX>> sig_handlers;

    ~ProcessData()
    {
        Memory::vfree(kernel_stack, kernel_stack_size/Memory::page_size());
    }
};

#endif // PROCESS_DATA_HPP
