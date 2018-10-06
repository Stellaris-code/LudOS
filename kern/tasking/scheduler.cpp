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

namespace tasking
{
DeltaQueue<pid_t> sleep_queue;

time_t elapsed_ticks { 0 };

void scheduler_init()
{
}

void update_sleep_queue()
{
    time_t current_ticks = Time::total_ticks();
    time_t microsec_duration = (current_ticks - elapsed_ticks) / (Time::clock_speed());
    elapsed_ticks = current_ticks;

    sleep_queue.decrease(microsec_duration);
}

bool process_ready(pid_t pid)
{
    return !sleep_queue.find(pid) && !Process::by_pid(pid)->is_waiting();
}

pid_t find_next_pid()
{
    pid_t next_pid = Process::enabled() ? Process::current().pid : 0;
    // while next_pid is a waiting pid, set next_pid to the next process in the process list
    do
    {
        next_pid = (next_pid + 1) % Process::count();
    } while (!Process::by_pid(next_pid) || !process_ready(next_pid));

    return next_pid;
}

void schedule()
{
    // FIXME : not proper wording ("active"?)
    if (Process::enabled()) Process::current().unswitch();

    update_sleep_queue();

    pid_t next_pid = find_next_pid();

    //log_serial("Switching from PID %d to PID %d (Process count : %d)\n", Process::current().pid, next_pid, Process::count());

    Process::by_pid(next_pid)->switch_to();
}

}
