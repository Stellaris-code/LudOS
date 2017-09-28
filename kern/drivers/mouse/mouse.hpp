/*
mouse.hpp

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
#ifndef MOUSE_HPP
#define MOUSE_HPP

#include <stdint.h>

struct MousePacket
{
    int16_t x;
    int16_t y;
    int16_t wheel;
    bool left_button : 1;
    bool mid_button : 1;
    bool right_button : 1;
    bool button_4 : 1;
    bool button_5 : 1;
};

struct MouseMoveEvent
{
    int16_t x;
    int16_t y;
};

struct MouseScrollEvent
{
    int16_t wheel;
};

struct MouseClickEvent
{
    bool left_button : 1;
    bool mid_button : 1;
    bool right_button : 1;
    bool button_4 : 1;
    bool button_5 : 1;
};

class Mouse
{
public:
    static void init();

    static bool left_but() { return left_pressed; }
    static bool mid_but() { return mid_pressed; }
    static bool right_but() { return right_pressed; }
    static bool fourth_but() { return fourth_pressed; }
    static bool fifth_but() { return fifth_pressed; }

private:
    static inline bool left_pressed { false };
    static inline bool mid_pressed { false };
    static inline bool right_pressed { false };
    static inline bool fourth_pressed { false };
    static inline bool fifth_pressed { false };
};

#endif // MOUSE_HPP
