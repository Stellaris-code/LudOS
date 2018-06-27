/*
serial.cpp

Copyright (c) 10 Yann BOUCHER (yann)

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

#include "serial.hpp"

#include "io.hpp"

#include "utils/bitops.hpp"
#include "utils/logging.hpp"

namespace serial
{

void set_divisor_rate(uint16_t comport, uint16_t rate)
{
    uint8_t line_ctl = inb(comport+LINE_CTL);
    bit_set(line_ctl, 7); // set DLAB
    outb(comport+LINE_CTL, line_ctl);

    uint8_t lsb = rate & 0xFF;
    uint8_t rsb = (rate >> 8) & 0xFF;

    outb(comport+LSB_DIV, lsb);
    outb(comport+RSB_DIV, rsb);

    bit_clear(line_ctl, 7);
    outb(comport+LINE_CTL, line_ctl);
}

void set_data_length(uint16_t comport, uint8_t length)
{
    uint8_t byte { 0 };

    switch (length)
    {
    case 5:
        byte = 0b00;
        break;
    case 6:
        byte = 0b01;
        break;
    case 7:
        byte = 0b10;
        break;
    case 8:
        byte = 0b11;
        break;

    default:
        warn("Invalid serial data length : %d\n", length);
        return;
    }

    uint8_t line_ctl = inb(comport+LINE_CTL);
    line_ctl &= 0b11111100;
    line_ctl &= byte;

    outb(comport+LINE_CTL, line_ctl);
}

void set_stop_bits(uint16_t comport, bool bits)
{
    uint8_t line_ctl = inb(comport+LINE_CTL);
    bit_change(line_ctl, bits, 2);
    outb(comport+LINE_CTL, line_ctl);
}

void set_parity(uint16_t comport, Parity parity)
{
    uint8_t line_ctl = inb(comport+LINE_CTL);
    line_ctl &= 0b11000111;
    line_ctl &= (parity << 3);
    outb(comport+LINE_CTL, line_ctl);
}

void set_interrupt_reg(uint16_t comport, uint8_t val)
{
    outb(comport+INT_REG, val);
}

void set_fifo(uint16_t comport, uint8_t val)
{
    outb(comport+IID_FCR_REG, val);
}

bool data_pending(uint16_t comport)
{
    return inb(comport + LINE_STATUS) & 1;
}

uint8_t read_serial(uint16_t comport)
{
    while (!data_pending(comport));

    return inb(comport);
}

bool transmission_pending(uint16_t comport)
{
    return !(inb(comport + LINE_STATUS) & 0x20);
}

void write_serial(uint16_t comport, uint8_t data)
{
    //while (transmission_pending(comport));

    outb(comport, data);
}

}
