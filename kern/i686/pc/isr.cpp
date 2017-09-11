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

#include "io.hpp"
#include "i686/pc/serialdebug.hpp"
#include "terminal/terminal.hpp"

#include <stdio.h>

isr::isr_t handlers[256] { nullptr };

extern "C"
const registers* isr_handler(const registers* const regs)
{
    if (regs->int_no == 8) // double fault
    {
        cli();
        // assume terminal is broken
        dump_serial(regs);
        serial::debug::write("Double fault, aborting\n");
        halt();
        return regs;
    }

    serial::debug::write("eip : 0x%lx\n", regs->eip);
    dump_serial(regs);
    dump(regs);
    panic("Unhandeld interrupt 0x%lx with error code 0x%lx at 0x%lx\n"
          "edx : 0x%lx\ncr2 : 0x%lx", regs->int_no, regs->err_code, regs->eip, regs->edx, cr2());
    // handle here

    return regs;
}

extern "C"
const registers* irq_handler(const registers* const regs)
{
    if (auto handl = handlers[regs->int_no]; handl)
    {
        handl(regs);
    }
    pic::send_eoi(regs->int_no-31);

    return regs;
}

void isr::register_handler(uint8_t num, isr::isr_t handler)
{
    handlers[num] = handler;
}
