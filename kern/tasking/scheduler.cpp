/*
scheduler.cpp

Copyright (c) 29 Yann BOUCHER (yann)

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

#include "scheduler.hpp"

#include "utils/logging.hpp"
#include "sys/time.h"
#include "time/time.hpp"
#include "tasking/process.hpp"

#include "i686/tasking/process.hpp"
#include "tasking/process_data.hpp"
#include "spinlock.hpp"

namespace tasking
{
DeltaQueue<pid_t, uint64_t> sleep_queue
{
    [](pid_t pid)
    {
        auto proc = Process::by_pid(pid);
        assert(proc);

        if (proc->status == Process::IOWait)
        {
            // execute the registered timeout action for this process if it exists
            if (proc->status_info.timeout_action)
                proc->status_info.timeout_action(proc, proc->status_info.timeout_action_arg);

            proc->status_info.timeout_action = nullptr;
        }
        else
        {
            assert(proc->status == Process::Sleeping);
        }

        proc->status = Process::Active;
    }
};

uint64_t elapsed_ticks { 0 };

void scheduler_init()
{
}

spinlock_t sleep_lock;
void update_sleep_queue()
{
    spin_lock(&sleep_lock);

    time_t current_ticks = Time::total_ticks();
    time_t tick_duration = current_ticks - elapsed_ticks;
    elapsed_ticks = current_ticks;

    //log_serial("tick duration : %lld\n", tick_duration);

    sleep_queue.decrease(tick_duration);

    spin_unlock(&sleep_lock);
}

bool process_ready(pid_t pid)
{
    return Process::by_pid(pid)->status == Process::Active;
}

pid_t find_next_pid()
{
    pid_t next_pid = Process::current().pid;
    // while next_pid is a waiting pid, set next_pid to the next process in the process list
    do
    {
        next_pid = (next_pid + 1) % Process::highest_pid();
    } while (!Process::by_pid(next_pid) || !process_ready(next_pid));

    if (next_pid == 0)
    {
        // FIXME : ugly
        //if next pid is idle task, search again to make sure we actually only have the idle task remaining
        do
        {
            next_pid = (next_pid + 1) % Process::highest_pid();
        } while (!Process::by_pid(next_pid) || !process_ready(next_pid));
    }

    return next_pid;
}

void delete_zombie_processes()
{
    for (pid_t pid { 0 }; pid < (pid_t)Process::highest_pid(); ++pid)
    {
        if (auto proc = Process::by_pid(pid); proc && proc->status == Process::Zombie && proc->pid != Process::current().pid)
        {
            Process::release_zombie(pid);
        }
    }
}

void schedule()
{
    delete_zombie_processes();

    update_sleep_queue();

    pid_t next_pid = find_next_pid();

    //log_serial("Switching from PID %d to PID %d (Process count : %d)\n", Process::current().pid, next_pid, Process::count());

    if (Process::current().pid != next_pid)
    {
        Process::task_switch(next_pid);
    }
}

}
