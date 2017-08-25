/*
multiboot_kern.cpp

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

#include "multiboot_kern.hpp"

#include <stdio.h>
#include "panic.hpp"

#include "utils/align.hpp"

namespace multiboot
{

void check(uint32_t magic, const multiboot_header &mbd, const multiboot_info* mbd_info)
{
    if (mbd.magic != MULTIBOOT2_HEADER_MAGIC || magic != MULTIBOOT2_BOOTLOADER_MAGIC)
    {
        printf("0x%08X\n", magic);
        panic("Multiboot2 Magic number is invalid ! Aborting");
        return;
    }
    if (reinterpret_cast<uintptr_t>(mbd_info) & 7)
    {
        printf ("Unaligned mbi: 0x%x\n", reinterpret_cast<uintptr_t>(mbd_info));
        return;
    }
}

void print_info(const multiboot_header &mbd, const multiboot_info* info)
{
    printf("%s", "Multiboot2 : architecture ");
    if      (mbd.architecture == MULTIBOOT_ARCHITECTURE_I386)   puts("32-bit i386 protected mode");
    else if (mbd.architecture == MULTIBOOT_ARCHITECTURE_MIPS32) puts("32-bit MIPS");
    else                                                        puts("Unknown");

    /*
        printf("Lower memory : 0x%08X\n", meminfo->mem_lower);
        printf("Upper memory : 0x%08X\n", meminfo->mem_upper);
    printf("Bootloader name : %s\n", info.bootloadername.string);
    printf("Command line : %s\n", info.commandline.string);
    */

    for (multiboot_tag* tag = reinterpret_cast<multiboot_tag*>(reinterpret_cast<uintptr_t>(info) + 8);
         tag->type != MULTIBOOT_TAG_TYPE_END;
         tag = reinterpret_cast<multiboot_tag*>(reinterpret_cast<multiboot_uint8_t *>(tag)
                                                + ((tag->size + 7) & ~7)))
    {
        switch(tag->type)
        {
        case MULTIBOOT_TAG_TYPE_CMDLINE:
            printf("Command line : "); print(reinterpret_cast<multiboot_tag_string*>(tag));
            break;
        case MULTIBOOT_TAG_TYPE_BOOT_LOADER_NAME:
            printf("Bootloader : "); print(reinterpret_cast<multiboot_tag_string*>(tag));
            break;
        case MULTIBOOT_TAG_TYPE_MMAP:
            print(reinterpret_cast<multiboot_tag_mmap*>(tag));
            break;
        case MULTIBOOT_TAG_TYPE_MODULE:
            print(reinterpret_cast<multiboot_tag_module*>(tag));
            break;
        case MULTIBOOT_TAG_TYPE_BOOTDEV:
            print(reinterpret_cast<multiboot_tag_bootdev*>(tag));
            break;
        default:
            ;
            //printf("(Ignored tag of type %d)\n", tag->type);
        }
    }


}

}
