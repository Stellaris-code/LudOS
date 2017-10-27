/*
ps2mouse.cpp

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

#include "ps2mouse.hpp"

#include <stdint.h>

#include "drivers/mouse/mouse.hpp"

#include "../interrupts/isr.hpp"
#include "pic.hpp"
#include "io.hpp"
#include "utils/messagebus.hpp"

#include "utils/logging.hpp"

void PS2Mouse::init()
{
    enable();

    isr::register_handler(IRQ12, &isr);

    log("Mouse driver initialized\n");
}

void PS2Mouse::isr(const registers *regs)
{
    uint8_t status = inb(COMMAND_PORT);
    size_t packet_counter { 0 };

    struct PS2MousePacket
    {
        uint8_t left_but : 1;
        uint8_t right_but : 1;
        uint8_t mid_but : 1;
        uint8_t always_one : 1;
        int8_t x_sign : 1;
        int8_t y_sign : 1;
        uint8_t x_overflow : 1;
        uint8_t y_overflow : 1;

        int8_t x;

        int8_t y;

        int8_t z : 4;
        uint8_t but_4 : 1;
        uint8_t but_5 : 1;
        uint8_t always_zero2 : 1;
        uint8_t always_zero1 : 1;
    };
    static_assert(sizeof(PS2MousePacket) == 4);

    uint8_t packed_array[4];

    while (status & 1)
    {
        int8_t mouse_in = inb(DATA_PORT);

        if (status & 0x20)
        {
            packed_array[packet_counter] = mouse_in;

            ++packet_counter;
        }
        status = inb(COMMAND_PORT);
    }

    if (!is_intellimouse)
    {
        packed_array[3] = 0; // set wheel parameters to null values
    }

    PS2MousePacket packet = *reinterpret_cast<PS2MousePacket*>(packed_array);

    if (packet.z >= 7) packet.z = 1;

    if (!packet.x_overflow && !packet.y_overflow && packet.always_one &&
            !(packet.z == -8 && (static_cast<int16_t>(packet.y_sign-static_cast<int16_t>(packet.y)) == 90
                                 || static_cast<int16_t>(packet.y_sign-static_cast<int16_t>(packet.y)) == 91))) // it oftens indicate a broken packet, dunno why
    {
        MessageBus::send<MousePacket>({static_cast<int16_t>(packet.x_sign-static_cast<int16_t>(packet.x)), static_cast<int16_t>(packet.y_sign-static_cast<int16_t>(packet.y)),
                                       static_cast<int16_t>(packet.z), static_cast<bool>(packet.left_but), static_cast<bool>(packet.mid_but), static_cast<bool>(packet.right_but),
                                       static_cast<bool>(packet.but_4), static_cast<bool>(packet.but_5)});
    }
}

void PS2Mouse::send_command(uint8_t command, bool poll)
{
    if (poll) poll_obf();
    outb(COMMAND_PORT, command);
}

void PS2Mouse::send_write(uint8_t val)
{
    poll_obf();
    outb(COMMAND_PORT, MOUSE_WRITE);
    poll_obf();
    outb(DATA_PORT, val);
}

uint8_t PS2Mouse::read()
{
    poll_ibf();
    return inb(DATA_PORT);
}

bool PS2Mouse::enable_intellimouse()
{
    set_sample_rate(200);
    set_sample_rate(100);
    set_sample_rate(80);

    poll_obf();
    send_write(GET_DEVICE_ID);
    read();

    bool result = read() == 0x03;

    set_sample_rate(40);

    return result;
}

void PS2Mouse::poll_ibf()
{
    uint32_t timeout = 100000/2;
    while (--timeout && !(inb(COMMAND_PORT) & 0x2));
}

void PS2Mouse::poll_obf()
{
    uint32_t timeout = 100000/2;
    while (--timeout && (inb(COMMAND_PORT) & 0x1) == 1);
}

void PS2Mouse::enable()
{
    pic::clear_mask(12); // enable mouse interrupts

    cli();

    poll_obf();
    send_command(MOUSE_ENABLE, false);

    poll_obf();
    send_command(IN_COMPAQ_BYTE, false);

    poll_ibf();
    uint8_t byte = inb(DATA_PORT);

    byte |= 0b10;
    byte &= 0b11011111;
    poll_obf();
    send_command(OUT_COMPAQ_BYTE, false);

    poll_obf();
    outb(DATA_PORT, byte);

    is_intellimouse = enable_intellimouse();

    if (is_intellimouse)
    {
        log("Intellimouse-compatible mouse detected\n");
    }

    send_write(SET_DEFAULTS);
    read();
    send_write(ENABLE_DATA_REPORTING);
    read();

    sti();
}

void PS2Mouse::disable()
{
    cli();

    pic::set_mask(12); // disable mouse interrupts
    send_command(MOUSE_DISABLE, false);

    sti();
}

void PS2Mouse::set_sample_rate(uint8_t rate)
{
    send_write(SET_SAMPLE_RATE);
    if (read() != 0xFA) return;

    send_write(rate);
    if (read() != 0xFA) return;
}
