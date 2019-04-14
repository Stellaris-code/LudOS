/*
semaphore.hpp

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
#ifndef SEMAPHORE_HPP
#define SEMAPHORE_HPP

#include <list.hpp>
#include "spinlock.hpp"

struct Process;

class Semaphore
{
public:
    Semaphore(int value = 0)
        : counter(value)
    {}
    ~Semaphore();

    void post();

    // if null, no timeout
    // returns ETIMEOUT if timed out
    int wait(uint64_t* timeout_ticks = nullptr);
    bool try_wait();

    int count() const;

private:
    void remove_timed_out_process(const Process* proc);
    static void remove_timed_out_process_entry_point(const Process* proc, void* semaphore);

private:
    struct wait_entry
    {
        Process* proc { nullptr };
        bool timed_out { false };
    };

private:
    spinlock_t lock = 0;
    volatile int counter = 0;
    std::list<wait_entry> wait_list;
};

#endif // SEMAPHORE_HPP
