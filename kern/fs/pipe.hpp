/*
pipe.hpp

Copyright (c) 14 Yann BOUCHER (yann)

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
#ifndef PIPE_HPP
#define PIPE_HPP

#include <deque.hpp>
#include <unordered_map.hpp>

#include "vfs.hpp"
#include "tasking/semaphore.hpp"

#include "utils/hash.hpp"

#include <sys/types.h>

namespace vfs
{

class pipe : public vfs::node
{
public:
    constexpr static size_t pipe_buf_size = 0x1000;

public:
    pipe()
        : node(nullptr)
    {}

    [[nodiscard]] virtual result<size_t> read_impl(size_t, gsl::span<uint8_t> data) const;
    [[nodiscard]] virtual result<kpp::dummy_t> write_impl(size_t, gsl::span<const uint8_t> data);

public:
    static std::shared_ptr<pipe> open_fifo(const std::shared_ptr<node>& node);

private:
    static std::unordered_map<std::pair<dev_t, ino_t>, std::shared_ptr<pipe>, pair_hash> named_pipes;

private:
    mutable std::deque<uint8_t> buffer {};
    mutable Semaphore can_read_sem  { 0 };
    mutable Semaphore can_write_sem { 0 };
};

}

#endif // PIPE_HPP
