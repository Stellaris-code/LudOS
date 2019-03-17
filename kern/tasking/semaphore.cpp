/*
semaphore.cpp

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

#include "semaphore.hpp"

#include "process.hpp"
#include "scheduler.hpp"
#include "atomic.hpp"

void Semaphore::wait()
{
    bool reschedule = false;

    LOCK(&lock);

    if (counter <= 0)
    {
        wait_list.push_back(&Process::current());
        Process::current().status = Process::IOWait;
        reschedule = true;
    }

    atomic_dec(&counter);

    UNLOCK(&lock);

    if (reschedule)
        tasking::schedule();
}

void Semaphore::post()
{
    bool reschedule = false;

    LOCK(&lock);

    atomic_inc(&counter);

    if (!wait_list.empty())
    {
        wait_list.front()->status = Process::Active;
        wait_list.pop_front();
        reschedule = true;
    }

    UNLOCK(&lock);

    if (reschedule)
        tasking::schedule();
}
