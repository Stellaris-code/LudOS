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
#include "utils/memutils.hpp"
#include "utils/env.hpp"
#include "utils/logging.hpp"
#include "utils/stlutils.hpp"
#include "elf/elf.hpp"
#include "mem/meminfo.hpp"
#include "mem/memmap.hpp"
#include "i686/mem/physallocator.hpp"
#include "i686/pc/mem/meminfo.hpp"
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
    if (CHECK_FLAG (info->flags, 6))
    {
        Meminfo::mmap_addr = (multiboot_mmap_entry*)Memory::mmap((void*)info->mmap_addr, info->mmap_length);
        Meminfo::mmap_length = info->mmap_length;
    }

    MemoryInfo::available_bytes = Meminfo::total_memory();
}

void parse_info()
{
    if (CHECK_FLAG(info->flags, 9))
    {
        if (get_str(info->boot_loader_name, 256) != "GRUB" && running_qemu)
        {
            running_qemu_kernel = true;
        }
    }

    if (CHECK_FLAG (info->flags, 3))
    {
        auto mmap_addr = (multiboot_module_t*)Memory::mmap((void*)info->mods_addr,
                                                     info->mods_count*sizeof(multiboot_module_t), Memory::Read);
        auto mod = mmap_addr;

        for (size_t i = 0; i < info->mods_count; i++, mod++)
        {
            PhysPageAllocator::mark_as_used(mod->mod_start, mod->mod_end-mod->mod_start);
        }

        Memory::unmap(mmap_addr, info->mods_count*sizeof(multiboot_module_t));
    }
}

void print_info()
{
    if (CHECK_FLAG(info->flags, 2))
    {
        log(Info, "Command line : '%s'\n", parse_cmdline().c_str());
    }
    if (CHECK_FLAG(info->flags, 1))
    {
        log(Info, "Boot device : 0x%x\n", static_cast<uint8_t>(info->boot_device>>24));
    }
    if (CHECK_FLAG (info->flags, 3))
    {
        log(Debug, "Module count : %d\n", info->mods_count);
        log(Debug, "Modules address : 0x%x\n", info->mods_addr);
        for (const auto& mod : get_modules())
        {
            log(Debug, " Module start : 0x%x\n", mod.mod_start);
            log(Debug, " Module end : 0x%x\n", mod.mod_end);
            log(Debug, " Module cmdline : '%s'\n", get_str(mod.cmdline, 512).c_str());
        }
    }
    if (CHECK_FLAG(info->flags, 5))
    {
        multiboot_elf_section_header_table_t elf_info = info->u.elf_sec;
        log(Debug, "Elf section addr : 0x%x\n", elf_info.addr);
    }
    if (CHECK_FLAG(info->flags, 9))
    {
        log(Info, "Bootloader name : '%s'\n", get_str(info->boot_loader_name, 256).c_str());
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

    auto shdr = (elf::Elf32_Shdr*)Memory::mmap((void*)elf_info.addr, elf_info.num*sizeof(elf::Elf32_Shdr), Memory::Read|Memory::Write);

    return {shdr, elf_info.num};
}

std::string parse_cmdline()
{
    if (CHECK_FLAG(info->flags, 2))
    {
        return get_str(info->cmdline, 256);
    }

    return "";
}

std::vector<multiboot_module_t> get_modules()
{
    if (CHECK_FLAG (info->flags, 6))
    {
        std::vector<multiboot_module_t> modules;

        auto mod = (multiboot_module_t*)Memory::mmap((void*)info->mods_addr,
                                                     info->mods_count*sizeof(multiboot_module_t), Memory::Read);

        for (size_t i = 0; i < info->mods_count; i++, mod++)
        {
            size_t len = mod->mod_end - mod->mod_start;

            modules.emplace_back(*mod);
            modules.back().mod_start = (uintptr_t)Memory::mmap((void*)mod->mod_start, len, Memory::Read);
            modules.back().mod_end = modules.back().mod_start + len;
            modules.back().cmdline = (uintptr_t)Memory::mmap((void*)mod->cmdline, 512, Memory::Read);
        }

        return modules;
    }
    else
    {
        return {};
    }
}

std::string get_str(uintptr_t addr, size_t size)
{
    return (char*)Memory::mmap((void*)addr, size, Memory::Read);
}

}
