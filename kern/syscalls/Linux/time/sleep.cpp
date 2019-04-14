/*
sleep.cpp

Copyright (c) 31 Yann BOUCHER (yann)

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

#include "tasking/scheduler.hpp"
#include "errno.h"
#include "utils/logging.hpp"

#include "time/time.hpp"

int sys_nanosleep(user_ptr<const struct timespec> req, user_ptr<struct timespec> rem)
{
    if (!req.check())
    {
        return -EFAULT;
    }
    if (rem.as_raw() != 0 && !rem.check())
    {
        return -EFAULT;
    }

    if (req.get()->tv_nsec < 0 || req.get()->tv_nsec > 999999999 || req.get()->tv_sec < 0)
    {
        return -EINVAL;
    }

    Process::current().status = Process::Sleeping;
    uint64_t ticks = (req.get()->tv_nsec/1000) * Time::clock_speed() + (req.get()->tv_sec * (Time::clock_speed()*1'000'000));

    tasking::sleep_queue.insert(Process::current().pid, ticks);

    tasking::schedule();

    return EOK;
}
