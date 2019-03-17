/*
pipe.cpp

Copyright (c) 16 Yann BOUCHER (yann)

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

#include "utils/user_ptr.hpp"

#include <errno.h>

#include "fs/vfs.hpp"
#include "fs/pipe.hpp"

#include "tasking/process.hpp"

int sys_pipe(user_ptr<int> fd)
{
    if (!fd.check())
    {
        return -EFAULT;
    }

    auto pipe = std::make_shared<vfs::pipe>();

    tasking::FDInfo fd1, fd2;
    fd1.node  =        fd2.node  = pipe ;
    fd1.read  = true ; fd2.read  = false;
    fd1.write = false; fd2.write = true ;

    fd.get()[0] = Process::current().add_fd(fd1);
    fd.get()[1] = Process::current().add_fd(fd2);

    return EOK;
}
