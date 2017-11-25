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

#include "panic.hpp"

#include "utils/align.hpp"
#include "utils/addr.hpp"
#include "utils/memutils.hpp"
#include "utils/env.hpp"
#include "utils/logging.hpp"
#include "utils/stlutils.hpp"
#include "elf/elf.hpp"
#include "mem/meminfo.hpp"
#include "i686/pc/mem/meminfo.hpp"
#include "i686/mem/paging.hpp"
#include "utils/virt_machine_detect.hpp"
#include "halt.hpp"

#define CHECK_FLAG(flags,bit)   ((flags) & (1 << (bit)))

namespace multiboot
{

const multiboot_info_t* info { nullptr };

void check(uint32_t magic, const multiboot_header &mbd, const multiboot_info* mbd_info)
{
    if (mbd.magic != MULTIBOOT_HEADER_MAGIC || magic != MULTIBOOT_BOOTLOADER_MAGIC)
    {
        log(Debug, "0x%lx\n", magic);
        panic("Multiboot2 Magic number is invalid ! Aborting");
        return;
    }
    if (reinterpret_cast<uintptr_t>(mbd_info) & 7)
    {
        log(Debug, "Unaligned mbi: 0x%lx\n", reinterpret_cast<uintptr_t>(mbd_info));
        return;
    }
}

void parse_mem()
{
    Meminfo::info = info;

    if (CHECK_FLAG (info->flags, 6))
    {
        Meminfo::mmap_addr = reinterpret_cast<multiboot_memory_map_t *>(phys(info->mmap_addr));
    }

    MemoryInfo::available_bytes = Meminfo::total_memory();
}

void parse_info()
{
    if (CHECK_FLAG(info->flags, 9))
    {
        if (strncmp(reinterpret_cast<char*>(phys(info->boot_loader_name)), "GRUB", 4) != 0 && running_qemu)
        {
            running_qemu_kernel = true;
        }
    }

    if (CHECK_FLAG (info->flags, 3))
    {
        multiboot_module_t * mod { reinterpret_cast<multiboot_module_t *>(phys(info->mods_addr)) };

        for (size_t i = 0; i < info->mods_count; i++, mod++)
        {
            Paging::mark_as_used(mod->mod_start, mod->mod_end-mod->mod_start);
        }
    }
}

void print_info()
{
    //puts("Multiboot Info :");

    //kprintf("Multiboot flags : 0x%x\n", info->flags);
    if (CHECK_FLAG(info->flags, 2))
    {
        log(Info, "Command line : '%s'\n", reinterpret_cast<char*>(phys(info->cmdline)));
    }
    if (CHECK_FLAG(info->flags, 1))
    {
        log(Info, "Boot device : 0x%x\n", static_cast<uint8_t>(info->boot_device>>24));
    }
    if (CHECK_FLAG (info->flags, 3))
    {
        multiboot_module_t * mod { reinterpret_cast<multiboot_module_t *>(phys(info->mods_addr)) };

        log(Debug, "Module count : %d\n", info->mods_count);
        log(Debug, "Modules address : 0x%x\n", info->mods_addr);
        for (size_t i = 0; i < info->mods_count; i++, mod++)
        {
            log(Debug, " Module start : 0x%x\n", mod->mod_start);
            log(Debug, " Module end : 0x%x\n", mod->mod_end);
            log(Debug, " Module cmdline : '%s'\n", reinterpret_cast<char*>(phys(mod->cmdline)));
        }
    }
    if (CHECK_FLAG(info->flags, 5))
    {
        multiboot_elf_section_header_table_t elf_info = info->u.elf_sec;
        log(Debug, "Elf section addr : 0x%x\n", elf_info.addr);
    }
    if (CHECK_FLAG(info->flags, 9))
    {
        log(Info, "Bootloader name : '%s'\n", reinterpret_cast<char*>(phys(info->boot_loader_name)));
    }

    if (CHECK_FLAG(info->flags, 12))
    {
        log(Info, "Video Framebuffer address : 0x%x\n", info->framebuffer_addr);
        log(Info, "Video type : %s\n", info->framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_RGB ? "RGB" :
                                                                                                  info->framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_INDEXED ? "Indexed" : "EGA Text");
        log(Info, "Video size : %dx%d\n", info->framebuffer_width, info->framebuffer_height);
    }
    else
    {
        log(Info, "No video mode associated, defaulting to 80x25 VGA text mode\n");
    }
}

std::pair<const elf::Elf32_Shdr *, size_t> elf_info()
{
    if (!CHECK_FLAG (info->flags, 5))
    {
        return {nullptr, 0};
    }

    multiboot_elf_section_header_table_t elf_info = info->u.elf_sec;
    auto shdr = reinterpret_cast<const elf::Elf32_Shdr*>(phys(elf_info.addr));

    return {shdr, elf_info.num};
}

std::string parse_cmdline()
{
    if (CHECK_FLAG(info->flags, 2))
    {
        return reinterpret_cast<char*>(phys(info->cmdline));
    }

    return "";
}

std::vector<multiboot_module_t> get_modules()
{
    if (CHECK_FLAG (info->flags, 6))
    {
        std::vector<multiboot_module_t> modules;

        multiboot_module_t * mod { reinterpret_cast<multiboot_module_t *>(phys(info->mods_addr)) };

        for (size_t i = 0; i < info->mods_count; i++, mod++)
        {
            modules.emplace_back(*mod);
        }

        return modules;
    }
    else
    {
        return {};
    }
}

}
