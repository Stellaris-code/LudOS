/*
waitpid.cpp

Copyright (c) 01 Yann BOUCHER (yann)

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
#include "syscalls/syscalls.hpp"

#include "tasking/process.hpp"
#include "tasking/process_data.hpp"

#include "tasking/scheduler.hpp"

#include <errno.h>

pid_t sys_waitpid(pid_t pid, user_ptr<int> wstatus, int options)
{
    if (!wstatus.check()) return -EFAULT;

    if (pid == -1)
    {
        Process::current().wait_for(pid, wstatus.get());
    }
    else if (pid > 0)
    {
        // pid doesn't exist or isn't a child
        if (Process::by_pid(pid) == nullptr || Process::by_pid(pid)->parent != Process::current().pid)
        {
            return -ECHILD;
        }
        Process::current().wait_for(pid, wstatus.get());
    }
    else
    {
        return -EINVAL;
    }

    tasking::schedule();
    // returns when the process is active again

    return Process::current().data->waitpid_child;
}
