/*
pipe.cpp

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

#include "pipe.hpp"

#include "fs/fs.hpp"

std::unordered_map<std::pair<dev_t, ino_t>, std::shared_ptr<vfs::pipe>, pair_hash> vfs::pipe::named_pipes;

vfs::node::result<size_t> vfs::pipe::read_impl(size_t, gsl::span<uint8_t> data) const
{
    // pipe is simply too small to contain such a large message atomically
    if (data.size() > pipe_buf_size)
        return kpp::make_unexpected(FSError{FSError::TooLarge});

    // wait while the pipe is large enough to read <size> bytes of data
    while (buffer.size() < data.size())
    {
        can_read_sem.wait();
    }

    // at this point the buffer should be large enough to allow the reading of <size> bytes
    assert(buffer.size() >= data.size());

    std::copy(buffer.end() - data.size(), buffer.end(), data.data());

    buffer.resize(buffer.size() - data.size()); // pop data

    can_write_sem.post();

    return {};
}

vfs::node::result<kpp::dummy_t> vfs::pipe::write_impl(size_t, gsl::span<const uint8_t> data)
{
    if (data.size() > pipe_buf_size)
        return kpp::make_unexpected(FSError{FSError::TooLarge});

    // if we can't fit the data in the rest of the buffer, wait for it to get consumed enough
    while (buffer.size() + data.size() > pipe_buf_size)
    {
        can_write_sem.wait();
    }

    // at this point we should be allowed to push data into the buffer
    assert(buffer.size() + data.size() < pipe_buf_size);

    for (size_t i { 0 }; i < data.size(); ++i)
        buffer.push_front(0);

    std::copy(data.begin(), data.end(), buffer.begin());

    can_read_sem.post();

    return {};
}

std::shared_ptr<vfs::pipe> vfs::pipe::open_fifo(const std::shared_ptr<vfs::node> &node)
{
    assert(node->type() == FIFO);

    dev_t device = node->get_fs() ? node->get_fs()->fs_id : 0;
    ino_t inode  = node->stat().inode;

    if (auto node = named_pipes[{device, inode}])
    {
        return node;
    }
    else
    {
        return named_pipes[{device, inode}] = std::make_shared<pipe>();
    }
}
