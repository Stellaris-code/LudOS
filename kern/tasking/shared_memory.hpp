/*
shared_memory.hpp

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
#ifndef SHARED_MEMORY_HPP
#define SHARED_MEMORY_HPP

#include <stdint.h>
#include <vector.hpp>

#include "utils/gsl/gsl_span.hpp"

#include "sys/types.h"

class SharedMemorySegment
{
public:
    SharedMemorySegment(pid_t initial_pid, size_t size_in_pages);

public:
    void attach(pid_t proc);
    void detach(pid_t proc);

    uintptr_t physical_address() const;

    gsl::span<const pid_t> attached_pids() const;

private:
    uintptr_t m_phys_addr { 0 };
    size_t m_size { 0 };
    std::vector<pid_t> m_attached_pids;
};

#endif // SHARED_MEMORY_HPP
