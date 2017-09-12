/*
serial.hpp

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
#ifndef SERIAL_HPP
#define SERIAL_HPP

#include <stdint.h>

#define DATA_REG     0
#define INT_REG      1
#define LSB_DIV      0
#define RSB_DIV      1
#define IID_FCR_REG  2
#define LINE_CTL     3
#define MODEM_CTL    4
#define LINE_STATUS  5
#define MODEM_STATUS 6
#define SCRATCH_REG  7

namespace serial
{

enum Parity
{
    None = 0b000,
    Odd = 0b001,
    Even = 0b011,
    Mark = 0b101,
    Space = 0b111
};

void set_divisor_rate(uint16_t comport, uint16_t rate);
void set_data_length(uint16_t comport, uint8_t length);
void set_stop_bits(uint16_t comport, bool bits);
void set_parity(uint16_t comport, Parity parity);
void set_interrupt_reg(uint16_t comport, uint8_t val);
void set_fifo(uint16_t comport, uint8_t val);

bool data_pending(uint16_t comport);
uint8_t read_serial(uint16_t comport);

bool transmission_pending(uint16_t comport);
void write_serial(uint16_t comport, uint8_t data);

}

#endif // SERIAL_HPP
