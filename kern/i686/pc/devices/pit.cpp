/*
pit.cpp

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

#include "pit.hpp"

#include <stdio.h>

#include <io.hpp>
#include "../isr.hpp"
#include "timer.hpp"

void PIT::init(uint32_t freq)
{
    isr::register_handler(IRQ0, &irq_callback);

    Timer::m_set_frequency_callback = &set_frequency;

    Timer::set_frequency(freq);
}

void PIT::set_frequency(uint32_t freq)
{
    // The value we send to the PIT is the value to divide it's input clock
    // (1193180 Hz) by, to get our required frequency. Important to note is
    // that the divisor must be small enough to fit into 16-bits.
    const uint32_t divisor = 1193180 / freq;

    // Send the command byte.
    outb(0x43, 0x36);

    // Divisor has to be sent byte-wise, so split here into upper/lower bytes.
    uint8_t l = (uint8_t)(divisor & 0xFF);
    uint8_t h = (uint8_t)( (divisor>>8) & 0xFF );

    // Send the frequency divisor.
    outb(0x40, l);
    outb(0x40, h);
}

void PIT::set_pcspeaker_frequency(uint16_t freq)
{
    const uint16_t div = 1193180 / freq;
    outb(0x43, 0xb6);
    outb(0x42, (uint8_t) (div) );
    outb(0x42, (uint8_t) (div >> 8));
}

void PIT::irq_callback(const registers * const)
{
    ++Timer::m_ticks;
}
