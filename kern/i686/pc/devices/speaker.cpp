/*
speaker.cpp

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

#include "speaker.hpp"
#include "io.hpp"
#include "pit.hpp"
#include "time/timer.hpp"

#include "utils/messagebus.hpp"

#include "drivers/sound/beep.hpp"

Speaker::Speaker()
{
    MessageBus::register_handler<BeepMessage>([this](const BeepMessage& msg)
    {
        beep_(msg.milliseconds);
    });
}

void Speaker::beep_(uint32_t time, uint16_t freq)
{
    play_sound(freq);
    Timer::register_callback(time, [this]{stop();});
}

void Speaker::play_sound(uint16_t freq)
{
    PIT::set_pcspeaker_frequency(freq);

    uint8_t tmp = inb(0x61);
    if (tmp != (tmp | 3))
    {
        outb(0x61, tmp | 3);
    }
}

void Speaker::stop()
{
    uint8_t tmp = inb(0x61) & 0xFC;

    outb(0x61, tmp);
}

ADD_DRIVER(Speaker)
