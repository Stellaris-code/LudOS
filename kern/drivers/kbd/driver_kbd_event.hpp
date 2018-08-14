/*
keystroke_event.hpp

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
#ifndef KEYSTROKE_EVENT_HPP
#define KEYSTROKE_EVENT_HPP

#include <stdint.h>

struct DriverKbdEvent
{
    size_t kdb_id;
    union
    {
        uint8_t pos;
        struct
        {
            uint8_t x : 5;
            uint8_t y : 3;
        };
    };

    enum
    {
        Pressed,
        Released
    } state;
};

namespace kbd
{

inline uint8_t pos(uint8_t row, uint8_t col)
{
    return ((col & 0b111) << 5) | (row & 0b11111);
}

}

#endif // KEYSTROKE_EVENT_HPP
