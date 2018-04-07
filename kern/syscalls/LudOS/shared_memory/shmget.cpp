/*
shmget.cpp

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
#include <sys/ipc.h>
#include <errno.h>

#include "tasking/process.hpp"
#include "tasking/shared_memory.hpp"

#include "utils/logging.hpp"

int sys_shmget(key_t key, size_t size, int shmflags)
{
    if (get_shared_mem(key))
    {
        if (shmflags & IPC_CREAT && shmflags & IPC_EXCL)
        {
            return -EEXIST;
        }

        return key;
    }
    else
    {
        if (!(shmflags & IPC_CREAT))
        {
            return -ENOENT;
        }

        if (key == IPC_PRIVATE) key = create_shared_memory_id();

        auto shm = create_shared_mem(key, size);
        if (!shm) return -ENOMEM;

        assert(!Process::current().data.shm_list.count(key));
        Process::current().data.shm_list[key].shm = shm;
        Process::current().data.shm_list[key].v_addr = 0; // not yet mapped

        return key;
    }
}
