/*
process.hpp

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
#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <string.hpp>
#include <vector.hpp>
#include <unordered_map.hpp>
#include <optional.hpp>
#include "utils/gsl/gsl_span.hpp"
#include "utils/noncopyable.hpp"

#include "sys/types.h"

namespace vfs
{
class node;
}

struct ProcessCreatedEvent
{
    pid_t pid;
};

struct ProcessDestroyedEvent
{
    pid_t pid;
    int err_code;
};

class SharedMemorySegment;

class Process : NonCopyable
{
public:
    struct ArchSpecificData;

    struct FDInfo
    {
        std::shared_ptr<vfs::node> node;
        bool read { false };
        bool write { false };
        bool append { false };
        size_t cursor { 0 };
    };

    enum class AccessRequestPerm : uint16_t
    {
        Read,
        Write,
        Exec
    };

    static constexpr size_t root_uid = 0;

public:
    static bool enabled();

    static Process* create(const std::vector<std::string> &args);
    static Process* clone(Process& proc);
    static Process& current();
    static size_t   count();
    static void     kill(pid_t pid, int err_code);
    static Process* by_pid(pid_t pid);

    static bool check_args_size(const std::vector<std::string> &args);

public:
    Process& operator=(Process&&) noexcept = default;
    Process(Process&& other) noexcept = default;

    ~Process();

    void reset(gsl::span<const uint8_t> code_to_copy, size_t allocated_size = 0);

    void set_args(const std::vector<std::string> &args);

    size_t add_fd(const FDInfo& info);
    FDInfo *get_fd(size_t fd);
    void close_fd(size_t fd);

    bool is_waiting() const;
    void wait_for(pid_t pid, int* wstatus);

    void switch_to();
    void unswitch();

    bool check_perms(uint16_t perms, uint16_t tgt_uid, uint16_t tgt_gid, AccessRequestPerm type);

    uintptr_t allocate_pages(size_t pages);
    bool      release_pages(uintptr_t ptr, size_t pages);

private:
    Process();

public:
    struct AllocatedPageEntry
    {
        uintptr_t paddr;
        uint32_t flags;
    };
    struct ShmEntry
    {
        std::shared_ptr<SharedMemorySegment> shm;
        void* v_addr;
    };

    std::string name { "<INVALID>" };
    pid_t pid { 0 };
    uint32_t uid { root_uid };
    uint32_t gid { 0 };

    pid_t parent { 0 };
    std::vector<pid_t> children;
    std::optional<pid_t> waiting_pid;
    int* wstatus { nullptr };
    uintptr_t waitstatus_phys { 0 };

    std::string pwd = "/";

    std::vector<FDInfo> fd_table;

    std::unordered_map<uintptr_t, AllocatedPageEntry> allocated_pages;

    std::vector<std::string> args;
    uintptr_t argv_phys_page;

    std::unordered_map<unsigned int, ShmEntry> m_shm_list;

    uintptr_t current_pc { 0 };
    ArchSpecificData* arch_data { nullptr };

private:
    void arch_init(gsl::span<const uint8_t> code_to_copy, size_t allocated_size);
    void map_shm();
    void unmap_shm();
    void unmap_user_space();
    void cleanup();
    void init_default_fds();
    void release_all_pages();
    void wake_up(pid_t child, int err_code);
    uintptr_t copy_argv_page();
    std::unordered_map<uintptr_t, AllocatedPageEntry> copy_allocated_pages();

    static pid_t find_free_pid();

private:
    static inline Process* m_current_process { nullptr };
    static inline std::vector<std::unique_ptr<Process>> m_processes;
    static inline size_t m_process_count { 0 };
};

extern "C" void test_task();

#endif // PROCESS_HPP
