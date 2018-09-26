/*
pic.cpp

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

#include "pic.hpp"

#include <stdio.h>

#include "io.hpp"
#include "panic.hpp"

namespace pic
{
void init()
{
    /* set up cascading mode */
    outb(PIC_MASTER_CMD, 0x10 + 0x01);
    io_wait();
    outb(PIC_SLAVE_CMD,  0x10 + 0x01);
    io_wait();

    /* Setup master's vector offset */
    outb(PIC_MASTER_DATA, 0x20);
    io_wait();
    /* Tell the slave its vector offset */
    outb(PIC_SLAVE_DATA, 0x28);
    io_wait();
    /* Tell the master that he has a slave */
    outb(PIC_MASTER_DATA, 4);
    io_wait();
    outb(PIC_SLAVE_DATA, 2);
    io_wait();
    /* Enabled 8086 mode */
    outb(PIC_MASTER_DATA, 0x01);
    io_wait();
    outb(PIC_SLAVE_DATA, 0x01);
    io_wait();

    outb(PIC_MASTER_DATA, 0);
    io_wait();
    outb(PIC_SLAVE_DATA, 0);
    io_wait();

    set_mask(1); // disable keyboard interrupts before keyboard initialization
}

void send_eoi(uint8_t irq)
{
    if(irq >= 8)
    {
        outb(PIC_SLAVE_CMD, PIC_CMD_EOI);
        io_wait();
    }
    outb(PIC_MASTER_CMD, PIC_CMD_EOI);
    io_wait();
}

void set_mask(uint8_t irq)
{
    uint16_t port;
    uint8_t value;

    if(irq < 8)
    {
        port = PIC_MASTER_DATA;
    }
    else
    {
        port = PIC_SLAVE_DATA;
        irq -= 8;
    }
    value = inb(port) | (1 << irq);
    outb(port, value);
}

void clear_mask(uint8_t irq)
{
    uint16_t port;
    uint8_t value;

    if(irq < 8)
    {
        port = PIC_MASTER_DATA;
    }
    else
    {
        port = PIC_SLAVE_DATA;
        irq -= 8;
    }
    value = inb(port) & ~(1 << irq);
    outb(port, value);
}

uint16_t get_mask()
{
    uint8_t master = inb(PIC_MASTER_DATA);
    uint8_t slave  = inb(PIC_SLAVE_DATA);

    return master | (slave << 8);
}

}
