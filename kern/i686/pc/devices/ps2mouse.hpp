/*
ps2mouse.hpp

Copyright (c) 27 Yann BOUCHER (yann)

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
#ifndef PS2MOUSE_HPP
#define PS2MOUSE_HPP

#include "i686/pc/cpu/registers.hpp"

#define MOUSE_ENABLE 0xA8
#define MOUSE_DISABLE 0xA7
#define MOUSE_RESEND 0xFE
#define MOUSE_WRITE 0xD4
#define COMMAND_PORT 0x64
#define IN_COMPAQ_BYTE 0x20
#define OUT_COMPAQ_BYTE 0x60
#define DATA_PORT 0x60
#define SET_DEFAULTS 0xF6
#define ENABLE_DATA_REPORTING 0xF4
#define SET_SAMPLE_RATE 0xF3
#define GET_DEVICE_ID 0xF2

class PS2Mouse
{
public:
    static void init();

    static void enable();
    static void disable();

    static void set_sample_rate(uint8_t rate);

private:
    static void isr(const registers* regs);

    static void send_command(uint8_t command, bool poll = true);
    static void send_write(uint8_t val);
    static uint8_t read();

    static bool enable_intellimouse();

    static void poll_ibf();
    static void poll_obf();

private:
    static inline bool is_intellimouse { false };
};

#endif // PS2MOUSE_HPP
