/*
isr.cpp

Copyright (c) 26 Yann BOUCHER (yann)

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

#include "registers.hpp"
#include "devices/pic.hpp"
#include "halt.hpp"
#include "panic.hpp"
#include "isr.hpp"

#include <stdio.h>

// TODO : print regs

isr::isr_t handlers[256] { nullptr };

extern "C"
const registers* isr_handler(const registers* const regs)
{
    panic("Unhandeld interrupt 0x%x with error code 0x%x at %p\n"
          "edx : 0x%x\n", regs->int_no, regs->err_code, regs->eip, regs->edx);
    // handle here

    return regs;
}

extern "C"
const registers* irq_handler(const registers* const regs)
{
    pic::send_eoi(regs->int_no-31);
    if (auto handl = handlers[regs->int_no]; handl)
    {
        handl(regs);
    }

    return regs;
}

void isr::register_handler(uint8_t num, isr::isr_t handler)
{
    handlers[num] = handler;
}
