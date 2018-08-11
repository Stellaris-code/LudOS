/*
interface.cpp

Copyright (c) 09 Yann BOUCHER (yann)

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

#include <errno.h>

#include "tasking/process.hpp"
#include "fs/vfs.hpp"

#include "utils/logging.hpp"

#include "utils/user_ptr.hpp"

int sys_get_interface(unsigned int fd, int interface_id, user_ptr<void> interface)
{
    auto entry = Process::current().get_fd(fd);
    if (!entry) return -EBADF;

    if (!interface.check()) return -EFAULT;

    if (!entry->node->implements(interface_id)) return -EINVAL;

    int interface_size = entry->node->get_interface(interface_id, interface.get());
    if (interface_size == -1) return -EINVAL;

    uintptr_t* interface_list = (uintptr_t*)interface.get();

    log_serial("The size is %d at %p\n", interface_size, interface_list);

    // Create individual user callbacks for each interface function
    for (size_t i { 0 }; i < interface_size/sizeof(uintptr_t); ++i)
    {
        const uintptr_t actual_callback = interface_list[i];

        interface_list[i] =
        Process::current().create_user_callback([actual_callback](void*){
            log_serial("Duuuuuuuuuude\n");
            while (true){}
            (*(void(*)())actual_callback)();
        });

        log_serial("Old 0x%x to new 0x%x\n", actual_callback, interface_list[i]);
    }

    return EOK;
}
