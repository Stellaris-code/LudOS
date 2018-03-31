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
#include "utils/gsl/gsl_span.hpp"
#include "utils/noncopyable.hpp"

namespace vfs
{
class node;
}

using pid_t = uint32_t;

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

    static Process* create(gsl::span<const std::string> args);
    static Process* clone(Process& proc);
    static Process& current();
    static size_t   count();
    static void     kill(pid_t pid);
    static Process* by_pid(pid_t pid);

    static bool check_args_size(gsl::span<const std::string> args);

public:
    Process& operator=(Process&&) noexcept = default;
    Process(Process&& other) noexcept = default;

    ~Process();

    void reset(gsl::span<const uint8_t> code_to_copy, size_t allocated_size = 0);

    void set_args(gsl::span<const std::string> args);

    size_t add_fd(const FDInfo& info);
    FDInfo *get_fd(size_t fd);
    void close_fd(size_t fd);

    void execute();
    void stop();

    bool check_perms(uint16_t perms, uint16_t tgt_uid, uint16_t tgt_gid, AccessRequestPerm type);

private:
    Process();

private:
    void init_default_fds();
    void release_allocated_pages();
    static pid_t find_free_pid();

public:
    std::string name { "<INVALID>" };
    pid_t pid { 0 };
    uint32_t uid { root_uid };
    uint32_t gid { 0 };
    std::string pwd = "/";
    std::vector<FDInfo> fd_table;
    std::vector<std::pair<uintptr_t, size_t>> allocated_pages;
    std::vector<std::string> args;
    uintptr_t start_address { 0 };
    ArchSpecificData* arch_data { nullptr };

private:
    void arch_init(gsl::span<const uint8_t> code_to_copy, size_t allocated_size);

private:
    static inline Process* m_current_process { nullptr };
    static inline std::vector<std::unique_ptr<Process>> m_processes;
    static inline size_t m_process_count { 0 };
};

extern "C" void test_task();

#endif // PROCESS_HPP
