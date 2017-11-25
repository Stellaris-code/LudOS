/*
syscommands.cpp

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

#include "syscommands.hpp"

#include "utils/memutils.hpp"

#include "shell/shell.hpp"
#include "mem/meminfo.hpp"

#include "external/liballoc/liballoc.h"

extern "C" unsigned long long l_allocated;		///< Running total of allocated memory.
extern "C" unsigned long long l_inuse;		///< Running total of used memory.

void install_sys_commands(Shell &sh)
{
    sh.register_command(
    {"meminfo", "Print memory info",
     "Usage : 'meminfo'",
     [](const std::vector<std::string>&)
     {
         kprintf("Total memory : %s\n", human_readable_size(MemoryInfo::available_bytes).c_str());
         kprintf("Free memory : %s\n", human_readable_size(MemoryInfo::available_bytes - l_inuse).c_str());
         kprintf("Allocated memory : %s\n", human_readable_size(l_allocated).c_str());
         kprintf("Used memory : %s\n", human_readable_size(l_inuse).c_str());
         return 0;
     }});

    sh.register_command(
    {"alloc_dump", "Print malloc info",
     "Usage : 'alloc_dump'",
     [](const std::vector<std::string>&)
     {
         liballoc_dump();
         return 0;
     }});
}
