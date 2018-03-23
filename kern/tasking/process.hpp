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

class Process : NonCopyable
{
public:
    struct ArchSpecificData;

    Process(gsl::span<const uint8_t> code_to_copy, size_t allocated_size = 0);
    Process(const std::string& name, gsl::span<const uint8_t> code_to_copy, size_t allocated_size = 0);
    ~Process(); // = default;

    static bool enabled();

    static Process& current();

public:
    struct FDInfo
    {
        std::shared_ptr<vfs::node> node;
        bool read { false };
        bool write { false };
        bool append { false };
        size_t cursor { 0 };
    };

    static constexpr size_t root_uid = 0;

public:
    size_t add_fd(const FDInfo& info);
    FDInfo *get_fd(size_t fd);
    void close_fd(size_t fd);

    void execute(gsl::span<const std::string> args);

    void stop();

private:
    void init_default_fds();

public:
    const std::string name { "<INVALID>" };
    const uint32_t id { 0 };
    const uint32_t uid { root_uid };
    const uint32_t gid { 0 };
    std::string pwd = "/";
    std::vector<FDInfo> fd_table;
    uintptr_t start_address { 0 };
    ArchSpecificData* arch_data { nullptr };

private:
    void arch_init(gsl::span<const uint8_t> code_to_copy, size_t allocated_size);

private:
    static inline Process* m_current_process { nullptr };
};

extern "C" void test_task();

#endif // PROCESS_HPP
