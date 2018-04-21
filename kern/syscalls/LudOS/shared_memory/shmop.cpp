/*
shmop.cpp

Copyright (c) 06 Yann BOUCHER (yann)

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
#include <sys/shm.h>
#include <errno.h>

#include "tasking/process.hpp"
#include "tasking/process_data.hpp"
#include "tasking/shared_memory.hpp"
#include "mem/memmap.hpp"
#include "utils/user_ptr.hpp"
#include "utils/stlutils.hpp"
#include "utils/logging.hpp"

long sys_shmat(int shmid, user_ptr<const void> shmaddr, int shmflg)
{
#ifndef LUDOS_HAS_SHM
    return -ENOSYS;
#else
    uintptr_t v_addr = (uintptr_t)shmaddr.bypass();

    if (!get_shared_mem(shmid) || v_addr >= KERNEL_VIRTUAL_BASE)
    {
        return -EINVAL;
    }

    if (v_addr != 0)
    {
        if (Memory::offset(v_addr) != 0 && !(shmflg & SHM_RND))
        {   // not rounded
            return -EINVAL;
        }
        v_addr += (Memory::page_size() - Memory::offset(v_addr));
        if (Memory::is_mapped((void*)v_addr))
        {
            return -EINVAL;
        }
    }
    else
    {
        v_addr = Memory::allocate_virtual_page(1, true);
        assert(!Memory::is_mapped((void*)v_addr));
    }

    Process::current().data->shm_list.at(shmid).v_addr = (void*)v_addr;
    Process::current().data->shm_list.at(shmid).shm->map((void*)v_addr);

    return v_addr;
#endif
}

long sys_shmdt(user_ptr<const void> shmaddr)
{
#ifndef LUDOS_HAS_SHM
    return -ENOSYS;
#else
    if (!shmaddr.check() || Memory::offset((uintptr_t)shmaddr.get()) != 0)
    {
        return -EINVAL;
    }
    uintptr_t v_addr = (uintptr_t)shmaddr.get();

    erase_if(Process::current().data->shm_list, [v_addr](const std::pair<unsigned int, tasking::ShmEntry>& pair)
    {
        return (uintptr_t)pair.second.v_addr == v_addr;
    });

    return 0;
#endif
}
