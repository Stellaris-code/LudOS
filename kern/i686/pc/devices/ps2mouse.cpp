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

#include "i686/interrupts/isr.hpp"
#include "pic.hpp"
#include "ps2controller.hpp"
#include "io.hpp"
#include "utils/messagebus.hpp"

#include "utils/logging.hpp"

enum PS2MouseCommands : uint8_t
{
    MouseEnable = 0xA8,
    MouseDisable = 0xA7,
    MouseResend = 0xFE,
    MouseWrite = 0xD4,
    InCompaqByte = 0x20,
    OutCompaqByte = 0x60,
    SetDefaults = 0xF6,
    EnableDataReporting = 0xF4,
    SetSampleRate = 0xF3,
    GetDeviceID = 0xF2
};

void PS2Mouse::init()
{
    enable();

    isr::register_handler(IRQ12, &isr);

    log(Info, "Mouse driver initialized\n");
}

bool PS2Mouse::isr(const registers *regs)
{
    uint8_t status = inb(CommandPort);
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
        int8_t mouse_in = inb(DataPort);

        if (status & 0x20)
        {
            packed_array[packet_counter] = mouse_in;

            ++packet_counter;
        }
        status = inb(CommandPort);
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

    return true;
}

void PS2Mouse::send_write(uint8_t val)
{
    PS2Controller::poll_obf();
    PS2Controller::send_command(MouseWrite);
    PS2Controller::poll_obf();
    outb(DataPort, val);
}

uint8_t PS2Mouse::read()
{
    PS2Controller::poll_ibf();
    return inb(DataPort);
}

bool PS2Mouse::enable_intellimouse()
{
    set_sample_rate(200);
    set_sample_rate(100);
    set_sample_rate(80);

    PS2Controller::poll_obf();
    send_write(GetDeviceID);
    read();

    bool result = read() == 0x03;

    set_sample_rate(40);

    return result;
}

void PS2Mouse::enable()
{
    pic::clear_mask(12); // enable mouse interrupts

    cli();

    PS2Controller::poll_obf();
    PS2Controller::send_command(MouseEnable, false);

    PS2Controller::poll_obf();
    PS2Controller::send_command(InCompaqByte, false);

    PS2Controller::poll_ibf();
    uint8_t byte = inb(DataPort);

    byte |= 0b10;
    byte &= 0b11011111;
    PS2Controller::poll_obf();
    PS2Controller::send_command(OutCompaqByte, false);

    PS2Controller::poll_obf();
    outb(DataPort, byte);

    is_intellimouse = enable_intellimouse();

    if (is_intellimouse)
    {
        log(Info, "Intellimouse-compatible mouse detected\n");
    }

    send_write(SetDefaults);
    read();
    send_write(EnableDataReporting);
    read();

    sti();
}

void PS2Mouse::disable()
{
    cli();

    pic::set_mask(12); // disable mouse interrupts
    PS2Controller::send_command(MouseDisable, false);

    sti();
}

void PS2Mouse::set_sample_rate(uint8_t rate)
{
    send_write(SetSampleRate);
    if (read() != 0xFA) return;

    send_write(rate);
    if (read() != 0xFA) return;
}
