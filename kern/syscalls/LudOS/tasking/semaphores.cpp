/*
semaphores.cpp

Copyright (c) 23 Yann BOUCHER (yann)

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

#include <sys/types.h>
#include <lud_semaphore.h>
#include <errno.h>
#include <sys/time.h>

#include <vector.hpp>

#include "tasking/semaphore.hpp"
#include "tasking/process.hpp"

#include "utils/user_ptr.hpp"
#include "time/time.hpp"
#include "utils/logging.hpp"

static std::vector<std::unique_ptr<Semaphore>> lud_sem_list;

Semaphore* get_sem(lud_sem_t id)
{
    if (id >= lud_sem_list.size())
        return nullptr;

    return lud_sem_list[id].get();
}

lud_sem_t create_new_sem(unsigned int value)
{
    for (size_t i { 0 }; i < lud_sem_list.size(); ++i)
    {
        if (lud_sem_list[i] == nullptr)
        {
            lud_sem_list[i] = std::make_unique<Semaphore>(value);
            return i;
        }
    }

    // expand list
    lud_sem_list.emplace_back(std::make_unique<Semaphore>(value));

    return lud_sem_list.size()-1;
}

void destroy_sem(lud_sem_t id)
{
    assert(get_sem(id));

    lud_sem_list[id].reset();
}


int sys_lud_sem_init(user_ptr<lud_sem_t> lud_sem_id, int pshared, unsigned int value)
{
    if (pshared)
        return -ENOSYS;
    if (!lud_sem_id.check())
        return -EFAULT;

    *lud_sem_id.get() = create_new_sem(value);

    return EOK;
}

int sys_lud_sem_destroy(user_ptr<lud_sem_t> lud_sem_id)
{
    if (!lud_sem_id.check())
        return -EFAULT;
    if (get_sem(*lud_sem_id.get()) == nullptr)
        return -EINVAL;

    destroy_sem(*lud_sem_id.get());

    return EOK;
}

int sys_lud_sem_post(user_ptr<lud_sem_t> lud_sem_id)
{
    if (!lud_sem_id.check())
        return -EFAULT;

    auto sem = get_sem(*lud_sem_id.get());
    if (sem == nullptr)
        return -EINVAL;

    sem->post();

    return EOK;
}

int sys_lud_sem_wait(user_ptr<lud_sem_t> lud_sem_id)
{
    if (!lud_sem_id.check())
        return -EFAULT;

    auto sem = get_sem(*lud_sem_id.get());
    if (sem == nullptr)
        return -EINVAL;

    sem->wait();

    return EOK;
}

int sys_lud_sem_trywait(user_ptr<lud_sem_t> lud_sem_id)
{
    if (!lud_sem_id.check())
        return -EFAULT;

    auto sem = get_sem(*lud_sem_id.get());
    if (sem == nullptr)
        return -EINVAL;

    if (!sem->try_wait())
    {
        return -EAGAIN;
    }

    return EOK;
}

int sys_lud_sem_timedwait(user_ptr<lud_sem_t> lud_sem_id, user_ptr<const struct timespec> tp)
{
    if (!lud_sem_id.check() || !tp.check())
        return -EFAULT;

    auto sem = get_sem(*lud_sem_id.get());
    if (sem == nullptr)
        return -EINVAL;

    uint64_t ticks = (tp.get()->tv_nsec/1000) * Time::clock_speed() + (tp.get()->tv_sec * (Time::clock_speed()*1'000'000));

    int ret = sem->wait(&ticks);

    if (ret == ETIMEDOUT)
        return -ETIMEDOUT;

    return EOK;
}

int sys_lud_sem_getvalue(user_ptr<lud_sem_t> lud_sem_id, user_ptr<int> sval)
{
    if (!lud_sem_id.check() || !sval.check())
        return -EFAULT;

    auto sem = get_sem(*lud_sem_id.get());
    if (sem == nullptr)
    {
        kprintf("PID %d, sem %d (%p) doesn't exist\n", Process::current().pid, *lud_sem_id.get(), lud_sem_id.as_raw());
        return -EINVAL;
    }

    *sval.get() = sem->count();

    return EOK;
}
