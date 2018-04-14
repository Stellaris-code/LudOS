/*
fsutils.hpp

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
#ifndef FSUTILS_HPP
#define FSUTILS_HPP

#include <memory.hpp>

#include <sys/types.h>
#include <errno.h>

class Process;

namespace vfs
{

class node;

struct QueryResult
{
    decltype(errno) error;
    std::shared_ptr<node> target_node;
};

std::shared_ptr<node> find(const std::string& path);
std::shared_ptr<node> find(const std::string& path, std::shared_ptr<vfs::node> search_root);

QueryResult user_find(const std::string& path);
QueryResult user_find(const std::string& path, Process& process);

[[nodiscard]] bool mount(std::shared_ptr<node> target, std::shared_ptr<node> mountpoint);
[[nodiscard]] bool umount(std::shared_ptr<node> target);

void traverse(const vfs::node& node, size_t indent = 0);
void traverse(const std::string& path);

[[nodiscard]] bool is_symlink(const vfs::node& node);
QueryResult resolve_symlink(const std::shared_ptr<vfs::node> link);
node& link_target(const vfs::node& link);
}

#endif // FSUTILS_HPP
