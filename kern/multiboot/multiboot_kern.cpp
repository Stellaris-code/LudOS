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

#include "utils/logging.hpp"
#include "panic.hpp"

#include "utils/align.hpp"
#include "utils/addr.hpp"

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

namespace multiboot
{

void check(uint32_t magic, const multiboot_header &mbd, const multiboot_info* mbd_info)
{
    if (mbd.magic != MULTIBOOT_HEADER_MAGIC || magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        log("0x%x\n", magic);
        panic("Multiboot2 Magic number is invalid ! Aborting");
        return;
    }
    if (reinterpret_cast<uintptr_t>(mbd_info) & 7)
    {
        log("Unaligned mbi: 0x%x\n", reinterpret_cast<uintptr_t>(mbd_info));
        return;
    }
}

void parse_info(const multiboot_info_t* info)
{
    //puts("Multiboot Info :");

    //printf("Multiboot flags : 0x%x\n", info->flags);
    if (CHECK_FLAG(info->flags, 1))
    {
    //    printf("Boot device : 0x%x\n", info->boot_device);
    }
    if (CHECK_FLAG(info->flags, 2))
    {
        log("Command line : '%s'\n", reinterpret_cast<char*>(phys(info->cmdline)));
    }
    /*if (CHECK_FLAG (info->flags, 3))
    {
        multiboot_module_t * mod { reinterpret_cast<multiboot_module_t *>(phys(info->mods_addr)) };

        printf("Module count : %d\n", info->mods_count);
        printf("Modules address : 0x%x\n", info->mods_addr);
        for (size_t i = 0; i < info->mods_count; i++, mod++)
        {
            printf(" Module start : 0x%x\n", mod->mod_start);
            printf(" Module end : 0x%x\n", mod->mod_end);
            printf(" Module cmdline : '%s'\n", reinterpret_cast<char*>(phys(mod->cmdline)));
        }
    }*/
    /*if (CHECK_FLAG (info->flags, 6))
    {
        for (multiboot_memory_map_t *mmap =reinterpret_cast<multiboot_memory_map_t *>(phys(info->mmap_addr));
             (uintptr_t)mmap < phys(info->mmap_addr) + info->mmap_length;
             mmap = (multiboot_memory_map_t *) ((unsigned long) mmap
                                                + mmap->size + sizeof (mmap->size)))
        {
            printf(" Base address : 0x%x, ", mmap->addr);
            printf("size : 0x%x, ", mmap->len);
            printf("type : %d\n", mmap->type);

        }
    }*/
    if (CHECK_FLAG(info->flags, 9))
    {
        log("Bootloader name : '%s'\n", reinterpret_cast<char*>(phys(info->boot_loader_name)));
    }
}

}
