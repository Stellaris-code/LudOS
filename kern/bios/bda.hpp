/*
bda.hpp

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
#ifndef BDA_HPP
#define BDA_HPP

#include "utils/stdint.h"

namespace BDA
{
    uint16_t com1_port();
    uint16_t com2_port();
    uint16_t com3_port();
    uint16_t com4_port();

    uint16_t lpt1_port();
    uint16_t lpt2_port();
    uint16_t lpt3_port();

    uint16_t equipement();

    uint16_t kbd_state();

    uint8_t *kbd_buffer(); // TODO : array !

    uint8_t disp_mode();

    uint16_t txt_mode_columns();

    uint16_t video_io_port();
}

#endif // BDA_HPP
