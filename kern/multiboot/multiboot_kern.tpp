/*
multiboot_kern.tpp

Copyright (c) 24 Yann BOUCHER (yann)

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
#ifndef MULTIBOOT_KERN_TPP
#define MULTIBOOT_KERN_TPP

#include <stdio.h>

namespace multiboot
{

template<>
inline void print<multiboot_tag_string>(const multiboot_tag_string *tag)
{
    printf("%s\n", tag->string);
}

template<>
inline void print<multiboot_tag_basic_meminfo>(const multiboot_tag_basic_meminfo *tag)
{
    printf("Lower memory : 0x%08X\n", tag->mem_lower);
    printf("Upper memory : 0x%08X\n", tag->mem_upper);
}

template<>
inline void print<multiboot_tag_mmap>(const multiboot_tag_mmap *tag)
{
    puts("Memory mapping : ");
    for (const multiboot_mmap_entry *mmap = tag->entries;
         (uintptr_t) mmap < (uintptr_t) tag + tag->size;
         mmap = (multiboot_mmap_entry *) ((uintptr_t) mmap + tag->entry_size))
    {
        printf (" base_addr = 0x%0X, ",
                mmap->addr);
        printf("length = 0x%0X, ", mmap->len);
        printf("type = %u\n", mmap->type);
    }
}

template<>
inline void print<multiboot_tag_module>(const multiboot_tag_module *tag)
{
    printf("Module Found : %s\n", tag->cmdline);
    printf("Module beggining : 0x%08X\n", tag->mod_start);
    printf("Module ending : 0x%08X\n", tag->mod_end);
}

template<>
inline void print<multiboot_tag_bootdev>(const multiboot_tag_bootdev *tag)
{
    printf("Boot device : %u\n", tag->biosdev);
    printf("Partition : %u\n", tag->part);
    printf("Slice : %u\n", tag->slice);
}
}

#endif // MULTIBOOT_KERN_TPP
