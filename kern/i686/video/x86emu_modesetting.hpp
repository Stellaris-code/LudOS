/*
x86emu_modesetting.hpp

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
#ifndef X86EMU_MODESETTING_HPP
#define X86EMU_MODESETTING_HPP

#include <stdint.h>

#include <vector.hpp>

#include "x86emu.h"
#include "utils/logging.hpp"

typedef union {
    uint32_t    Ptr32;
    struct {
        uint16_t    Off;
        uint16_t    Seg;
    };
} rmode_ptr;

struct RealModeState
{
    struct RAII
    {
        ~RAII()
        {
            x86emu_done(emu);
        }
        x86emu_t* emu;
    } cpu_state;
};

extern uint8_t* emu_mem;

void init_emu_mem();

uintptr_t translate_address(rmode_ptr ptr);
uint8_t* read_address(rmode_ptr ptr);

RealModeState emuInt10h(uint16_t ax, uint16_t bx = 0, uint16_t cx = 0, uint16_t dx = 0, uint16_t es = 0);

#endif // X86EMU_MODESETTING_HPP
