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

#include <errno.h>

#include "process.hpp"
#include "scheduler.hpp"
#include "atomic.hpp"

int Semaphore::wait(uint64_t *timeout_ticks)
{
    int result = EOK;
    bool reschedule = false;
    decltype(wait_list)::iterator waitlist_entry;

    spin_lock(&lock);

    if (atomic_load(&counter) <= 0)
    {
        wait_list.push_back({&Process::current(), false});
        waitlist_entry = --wait_list.end();
        Process::current().status = Process::IOWait;

        // if a timeout was set, register it
        if (timeout_ticks)
        {
            Process::current().status_info.timeout_action = Semaphore::remove_timed_out_process_entry_point;
            Process::current().status_info.timeout_action_arg = this;

            tasking::sleep_queue.insert(Process::current().pid, *timeout_ticks);
        }

        reschedule = true;
    }

    atomic_dec(&counter);

    spin_unlock(&lock);

    if (reschedule)
    {
        tasking::schedule();

        if (waitlist_entry->timed_out)
            result = ETIMEDOUT;
        // cleanup
        wait_list.erase(waitlist_entry);
    }

    return result;
}

bool Semaphore::try_wait()
{
    spin_lock(&lock);

    if (atomic_load(&counter) <= 0)
    {
        spin_unlock(&lock);
        return false;
    }

    atomic_dec(&counter);

    spin_unlock(&lock);
    return true;
}

int Semaphore::count() const
{
    return atomic_load(&counter);
}

Semaphore::~Semaphore()
{
    assert(wait_list.empty());
}

void Semaphore::post()
{
    bool reschedule = false;

    spin_lock(&lock);

    atomic_inc(&counter);

    if (!wait_list.empty())
    {
        wait_list.front().proc->status = Process::Active;
        //wait_list.pop_front();
        reschedule = true;
    }

    spin_unlock(&lock);

    if (reschedule)
        tasking::schedule();
}

void Semaphore::remove_timed_out_process(const Process *proc)
{
    auto it = std::find_if(wait_list.begin(), wait_list.end(), [proc](const wait_entry& entry)
    {
        return entry.proc->pid == proc->pid;
    });
    assert(it != wait_list.end());

    it->timed_out = true;
}

void Semaphore::remove_timed_out_process_entry_point(const Process *proc, void *semaphore)
{
    assert(semaphore);
    ((Semaphore*)semaphore)->remove_timed_out_process(proc);
}
