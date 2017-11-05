/*
x86emu_modesetting.cpp

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

#include "x86emu_modesetting.hpp"

#include "io.hpp"
#include "utils/addr.hpp"
#include "utils/logging.hpp"
#include "utils/memutils.hpp"
#include "halt.hpp"
#include "panic.hpp"

#include <array.hpp>
#include <string.hpp>

std::vector<uint8_t> emu_mem;
bool x86_flag = false;

void init_emu_mem()
{
    emu_mem.resize(0x100000);
    for (size_t i { 0 }; i < emu_mem.size(); ++i)
    {
        emu_mem[i] = *reinterpret_cast<uint8_t*>(phys(i));
    }
}

unsigned alternate_vm_memio(x86emu_t *emu, uint32_t addr, uint32_t *val, uint32_t type)
{
    x86emu_mem_t *mem = emu->mem;

    uint32_t bits = type & 0xFF;
    type &= ~0xFF;

    mem->invalid = 0;
    if (addr >= emu_mem.size())
    {
        panic("Out of bounds ! \n");
    }

    switch(type)
    {
        case X86EMU_MEMIO_R:
            switch(bits)
            {
                case X86EMU_MEMIO_8:
                    *val = *reinterpret_cast<uint8_t*>(emu_mem.data() + addr);
                    break;
                case X86EMU_MEMIO_16:
                    *val = *reinterpret_cast<uint16_t*>(emu_mem.data() + addr);
                    break;
                case X86EMU_MEMIO_32:
                    *val = *reinterpret_cast<uint32_t*>(emu_mem.data() + addr);
                    break;
                case X86EMU_MEMIO_8_NOPERM:
                    *val = *reinterpret_cast<uint8_t*>(emu_mem.data() + addr);
                    break;
            }
            break;

        case X86EMU_MEMIO_W:
            switch(bits)
            {
                case X86EMU_MEMIO_8:
                    *reinterpret_cast<uint8_t*>(emu_mem.data() + addr) = *val;
                    break;
                case X86EMU_MEMIO_16:
                    *reinterpret_cast<uint16_t*>(emu_mem.data() + addr) = *val;
                    break;
                case X86EMU_MEMIO_32:
                    *reinterpret_cast<uint32_t*>(emu_mem.data() + addr) = *val;
                    break;
                case X86EMU_MEMIO_8_NOPERM:
                    *reinterpret_cast<uint8_t*>(emu_mem.data() + addr) = *val;
                    break;
            }
            break;

        case X86EMU_MEMIO_X:
            switch(bits)
            {
                case X86EMU_MEMIO_8:
                    *val = *reinterpret_cast<uint8_t*>(emu_mem.data() + addr);
                    break;
                case X86EMU_MEMIO_16:
                    *val = *reinterpret_cast<uint16_t*>(emu_mem.data() + addr);
                    break;
                case X86EMU_MEMIO_32:
                    *val = *reinterpret_cast<uint32_t*>(emu_mem.data() + addr);
                    break;
            }
            break;

        case X86EMU_MEMIO_I:
            //log(Debug , "In : 0x%x\n", addr);
            switch(bits)
            {
                case X86EMU_MEMIO_8:
                    *val = inb(addr);
                    break;
                case X86EMU_MEMIO_16:
                    *val = inw(addr);
                    break;
                case X86EMU_MEMIO_32:
                    *val = inl(addr);
                    break;
            }
            break;

        case X86EMU_MEMIO_O:
            //log(Debug , "Out : 0x%x:0x%x\n", addr, *val);
            switch(bits)
            {
                case X86EMU_MEMIO_8:
                    outb(addr, *val);
                    break;
                case X86EMU_MEMIO_16:
                    outw(addr, *val);
                    break;
                case X86EMU_MEMIO_32:
                    outl(addr, *val);
                    break;
            }
            break;
    }

    return 0;
}

// Source : http://forum.osdev.org/viewtopic.php?f=1&t=31388
RealModeState emuInt10h(uint16_t ax, uint16_t bx, uint16_t cx, uint16_t dx, uint16_t es)
{
    x86emu_t* emu = x86emu_new(X86EMU_PERM_RWX, X86EMU_PERM_RWX);

    x86emu_set_log(emu, 100, [](x86emu_t *emu, char *buf, unsigned size)->void
    {
        std::string str(buf, size);
        err("x86emu : %s\n", str.c_str());
    });

    x86emu_set_memio_handler(emu, reinterpret_cast<x86emu_memio_handler_t>(alternate_vm_memio));

    emu->x86.R_IP = 0x7C00;

    *reinterpret_cast<uint8_t*>(emu_mem.data() + 0x7C00) = 0x90;    // nop
    *reinterpret_cast<uint8_t*>(emu_mem.data() + 0x7C01) =  0xF4;   // hlt

    emu->x86.R_AX = ax;
    emu->x86.R_BX = bx;
    emu->x86.R_CX = cx;
    emu->x86.R_DX = dx;

    x86emu_set_seg_register(emu, emu->x86.R_DS_SEL, 0);
    x86emu_set_seg_register(emu, emu->x86.R_CS_SEL, 0);
    x86emu_set_seg_register(emu, emu->x86.R_SS_SEL, 0x8000);
    x86emu_set_seg_register(emu, emu->x86.R_ES_SEL, es);

    x86emu_intr_raise(emu, 0x10, INTR_TYPE_SOFT, 0);

    x86emu_run(emu, X86EMU_RUN_LOOP);

    return {{emu}};
}

uintptr_t translate_address(rmode_ptr ptr)
{
    return ptr.Seg * 0x10 + ptr.Off;
}

uint8_t *read_address(rmode_ptr ptr)
{
    return emu_mem.data() + translate_address(ptr);
}
