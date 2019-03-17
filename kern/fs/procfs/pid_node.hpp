/*
pid_node.hpp

Copyright (c) 10 Yann BOUCHER (yann)

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
#ifndef PID_NODE_HPP
#define PID_NODE_HPP

#include "fs/vfs.hpp"

#include <sys/types.h>

namespace procfs
{

struct pid_node : public vfs::node
{
public:
    pid_node(node* parent, pid_t pid)
        : node(parent), m_pid(pid)
    {}

public:
    virtual Type type() const override { return Directory; }
    virtual kpp::string name() const override;
    virtual std::vector<std::shared_ptr<node>> readdir_impl() override;

private:
    pid_t m_pid;
};

}

#endif // PID_NODE_HPP
