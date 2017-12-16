/*
bda.cpp

Copyright (c) 23 Yann BOUCHER (yann)

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

#include "bda.hpp"

uint16_t BDA::com1_port()
{
    return *reinterpret_cast<uint16_t*>(0x0400);
}

uint16_t BDA::com2_port()
{
    return *reinterpret_cast<uint16_t*>(0x0402);
}

uint16_t BDA::com3_port()
{
    return *reinterpret_cast<uint16_t*>(0x0404);
}

uint16_t BDA::com4_port()
{
    return *reinterpret_cast<uint16_t*>(0x0406);
}


uint16_t BDA::lpt1_port()
{
    return *reinterpret_cast<uint16_t*>(0x0408);
}

uint16_t BDA::lpt2_port()
{
    return *reinterpret_cast<uint16_t*>(0x040A);
}

uint16_t BDA::lpt3_port()
{
    return *reinterpret_cast<uint16_t*>(0x040C);
}

uint16_t BDA::equipement()
{
    return *reinterpret_cast<uint16_t*>(0x0410);
}

uint16_t BDA::kbd_state()
{
    return *reinterpret_cast<uint16_t*>(0x0417);
}

uint8_t *BDA::kbd_buffer()
{
    return  reinterpret_cast<uint8_t*>(0x041E);
}

uint8_t BDA::disp_mode()
{
    return *reinterpret_cast<uint8_t*>(0x0449);
}

uint16_t BDA::txt_mode_columns()
{
    return *reinterpret_cast<uint16_t*>(0x044A);
}

uint16_t BDA::video_io_port()
{
    return *reinterpret_cast<uint16_t*>(0x0463);
}
