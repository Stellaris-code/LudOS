/*
meminfo.hpp

Copyright (c) 31 Yann BOUCHER (yann)

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
#ifndef MEMINFO_HPP
#define MEMINFO_HPP

#include <stdint.h>

struct multiboot_mmap_entry;
typedef struct multiboot_mmap_entry multiboot_memory_map_t;
struct multiboot_info;
typedef struct multiboot_info multiboot_info_t;

namespace multiboot
{
void parse_mem(const multiboot_info_t* info);
void parse_mem(const multiboot_info_t* info);
}

class Meminfo
{
    friend void multiboot::parse_mem(const multiboot_info_t* info);

public:
    static size_t free_frames();
    static multiboot_memory_map_t *largest_frame();
    static multiboot_memory_map_t *frame(size_t idx);

    static void init_paging_bitmap();

private:
    static inline multiboot_mmap_entry *mmap_addr;
    static inline const multiboot_info_t* info;
};

#endif // MEMINFO_HPP
