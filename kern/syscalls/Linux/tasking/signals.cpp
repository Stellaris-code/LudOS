/*
signals.cpp

Copyright (c) 15 Yann BOUCHER (yann)

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

#include <signal.h>
#include <errno.h>

#include "tasking/process.hpp"
#include "tasking/process_data.hpp"

#include "utils/user_ptr.hpp"

sighandler_t sys_signal(int num, user_ptr<sighandler_noptr_t> handler)
{
    if (num < 0 || num >= SIGRTMAX || num == SIGTERM || num == SIGSTOP)
    {
        return (sighandler_t)-EINVAL;
    }

    auto& table = *Process::current().data->sig_handlers;
    if (handler.bypass() == SIG_DFL) table[num].sa_handler = (sighandler_t)Process::default_sighandler_actions[num];
    else if (handler.bypass() == SIG_IGN) table[num].sa_handler = (sighandler_t)SIG_ACTION_IGN;
    else
    {
        if (!handler.check())
        {
            return (sighandler_t)-EFAULT;
        }

        table[num].sa_handler = handler.get();
    }

    return handler.bypass();
}

int sys_sigaction(int num, user_ptr<const struct sigaction> act, user_ptr<struct sigaction> oldact)
{
    if (num < 0 || num >= SIGRTMAX || num == SIGTERM || num == SIGSTOP)
    {
        return -EINVAL;
    }

    if (!act.check() || (oldact.bypass() != nullptr && !oldact.check()))
    {
        return -EFAULT;
    }

    auto& table = *Process::current().data->sig_handlers;
    if (act.get()->sa_handler == SIG_DFL) table[num].sa_handler = (sighandler_t)Process::default_sighandler_actions[num];
    else if (act.get()->sa_handler == SIG_IGN) table[num].sa_handler = (sighandler_t)SIG_ACTION_IGN;
    else
    {
        if (Memory::check_user_ptr((const void*)act.get()->sa_handler, sizeof(sighandler_t)))
        {
            return -EFAULT;
        }

        table[num].sa_handler = act.get()->sa_handler;
    }

    return EOK;
}
