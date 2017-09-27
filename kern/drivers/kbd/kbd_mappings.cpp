/*
kbd_mappings.cpp

Copyright (c) 24 Yann BOUCHER (yann)

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

#include "kbd_mappings.hpp"

void kbd::install_mapping(const kbd::Mapping &mapping)
{
    if (current_handle)
    {
        MessageBus::remove_handler(current_handle.value());
    }

    MessageBus::register_handler<DriverKbdEvent>([mapping](const DriverKbdEvent& e)
    {
        if (mapping[e.pos])
        {
            MessageBus::send<KeyEvent>({mapping[e.pos], e.state == DriverKbdEvent::Pressed ? KeyEvent::Pressed : KeyEvent::Released});
        }
    });
}

kbd::Mapping kbd::mappings::azerty()
{
    Mapping map;
    auto def = [&map](uint8_t x, uint8_t y, Key key)
    {
        map[pos(x, y)] = key;
    };

    def(0, 0, Escape);
    def(1, 2, A);
    def(2, 2, Z);
    def(3, 2, E);
    def(4, 2, R);
    def(5, 2, T);
    def(6, 2, Y);
    def(7, 2, U);
    def(8, 2, I);
    def(9, 2, O);
    def(10, 2, P);
    def(1, 3, Q);
    def(2, 3, S);
    def(3, 3, D);
    def(4, 3, F);
    def(5, 3, G);
    def(6, 3, H);
    def(7, 3, J);
    def(8, 3, K);
    def(9, 3, L);
    def(10, 3, M);
    def(2, 4, W);
    def(3, 4, X);
    def(4, 4, C);
    def(5, 4, V);
    def(6, 4, B);
    def(7, 4, N);

    def(17, 5, Numpad0);
    def(17, 4, Numpad1);
    def(18, 4, Numpad2);
    def(19, 4, Numpad3);
    def(17, 3, Numpad4);
    def(18, 3, Numpad5);
    def(19, 3, Numpad6);
    def(17, 2, Numpad7);
    def(18, 2, Numpad8);
    def(19, 2, Numpad9);

    def(1, 0, F1);
    def(2, 0, F2);
    def(3, 0, F3);
    def(4, 0, F4);
    def(5, 0, F5);
    def(6, 0, F6);
    def(7, 0, F7);
    def(8, 0, F8);
    def(9, 0, F9);
    def(10, 0, F10);
    def(11, 0, F11);
    def(12, 0, F12);

    def(15, 4, Up);
    def(15, 5, Down);
    def(14, 5, Left);
    def(16, 5, Right);

    def(18, 1, Divide);
    def(19, 1, Multiply);
    def(20, 1, Subtract);
    def(20, 3, Add);

    def(16, 1, PageUp);
    def(16, 2, PageDown);
    def(15, 2, End);
    def(15, 1, Home);
    def(14, 1, Insert);
    def(14, 2, Delete);

    def(3, 5,  Space);
    def(12, 3, Return);
    def(13, 1, BackSpace);
    def(0, 2,  Tab);

    def(10, 4, Slash);
    def(13, 2, BackSlash);
    def(0, 1,  Tilde);
    def(12, 1, Equal);
    def(11, 1, Dash);

    def(9, 4, SemiColon);
    def(8, 4,  Comma);
    def(19, 5, Period);
    def(11, 3, Quote);

    def(11, 2, LBracket);
    def(12, 2, RBracket);

    def(13, 5, RControl);
    def(12, 4, RShift);
    def(10, 5, RAlt);

    def(0, 5, LControl);
    def(0, 4, LShift);
    def(2, 5, LAlt);

    def(11, 4, Bang);

    def(1, 1, Num1);
    def(2, 1, Num2);
    def(3, 1, Num3);
    def(4, 1, Num4);
    def(5, 1, Num5);
    def(6, 1, Num6);
    def(7, 1, Num7);
    def(8, 1, Num8);
    def(9, 1, Num9);
    def(10,1, Num0);

    def(0, 3, MajLock);
    def(14, 0, PrintScreen);
    def(15, 0, ScrollLock);
    def(17, 1, NumLock);

    return map;
}
