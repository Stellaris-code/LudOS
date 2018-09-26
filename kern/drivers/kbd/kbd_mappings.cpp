/*
kbd_mappings.cpp

Copyright (c) 24 Yann BOUCHER (yann)

Permission is hereby granted, Keyfree of charge, Keyto any person obtaining a copy
of this software and associated documentation files (the "Software"), Keyto deal
in the Software without restriction, Keyincluding without limitation the rights
to use, Keycopy, Keymodify, Keymerge, Keypublish, Keydistribute, Keysublicense, Keyand/or sell
copies of the Software, Keyand to permit persons to whom the Software is
furnished to do so, Keysubject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", KeyWITHOUT WARRANTY OF ANY KIND, KeyEXPRESS OR
IMPLIED, KeyINCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, KeyDAMAGES OR OTHER
LIABILITY, KeyWHETHER IN AN ACTION OF CONTRACT, KeyTORT OR OTHERWISE, KeyARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "kbd_mappings.hpp"

void kbd::install_mapping(const kbd::Mapping &mapping, size_t kbd_id)
{
    if (current_handle[kbd_id])
    {
        kmsgbus.remove_handler(current_handle[kbd_id].value());
    }

    kmsgbus.register_handler<DriverKbdEvent>([mapping, kbd_id](const DriverKbdEvent& e)
    {
        if (e.kdb_id == kbd_id && mapping[e.pos])
        {
            kmsgbus.send<KeyEvent>({e.kdb_id, mapping[e.pos], e.state == DriverKbdEvent::Pressed ? KeyEvent::Pressed : KeyEvent::Released});
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

    def(0, 0, KeyEscape);
    def(1, 2, KeyA);
    def(2, 2, KeyZ);
    def(3, 2, KeyE);
    def(4, 2, KeyR);
    def(5, 2, KeyT);
    def(6, 2, KeyY);
    def(7, 2, KeyU);
    def(8, 2, KeyI);
    def(9, 2, KeyO);
    def(10, 2,KeyP);
    def(1, 3, KeyQ);
    def(2, 3, KeyS);
    def(3, 3, KeyD);
    def(4, 3, KeyF);
    def(5, 3, KeyG);
    def(6, 3, KeyH);
    def(7, 3, KeyJ);
    def(8, 3, KeyK);
    def(9, 3, KeyL);
    def(10, 3,KeyM);
    def(2, 4, KeyW);
    def(3, 4, KeyX);
    def(4, 4, KeyC);
    def(5, 4, KeyV);
    def(6, 4, KeyB);
    def(7, 4, KeyN);

    def(17, 5, KeyNumpad0);
    def(17, 4, KeyNumpad1);
    def(18, 4, KeyNumpad2);
    def(19, 4, KeyNumpad3);
    def(17, 3, KeyNumpad4);
    def(18, 3, KeyNumpad5);
    def(19, 3, KeyNumpad6);
    def(17, 2, KeyNumpad7);
    def(18, 2, KeyNumpad8);
    def(19, 2, KeyNumpad9);

    def(1, 0, KeyF1);
    def(2, 0, KeyF2);
    def(3, 0, KeyF3);
    def(4, 0, KeyF4);
    def(5, 0, KeyF5);
    def(6, 0, KeyF6);
    def(7, 0, KeyF7);
    def(8, 0, KeyF8);
    def(9, 0, KeyF9);
    def(10, 0, KeyF10);
    def(11, 0, KeyF11);
    def(12, 0, KeyF12);

    def(15, 4, KeyUp);
    def(15, 5, KeyDown);
    def(14, 5, KeyLeft);
    def(16, 5, KeyRight);

    def(18, 1, KeyDivide);
    def(19, 1, KeyMultiply);
    def(20, 1, KeySubtract);
    def(20, 3, KeyAdd);

    def(16, 1, KeyPageUp);
    def(16, 2, KeyPageDown);
    def(15, 2, KeyEnd);
    def(15, 1, KeyHome);
    def(14, 1, KeyInsert);
    def(14, 2, KeyDelete);

    def(3, 5, KeySpace);
    def(12, 3, KeyReturn);
    def(13, 1, KeyBackSpace);
    def(0, 2, KeyTab);

    def(10, 4, KeySlash);
    def(13, 2, KeyBackSlash);
    def(0, 1, KeyTilde);
    def(12, 1, KeyEqual);
    def(11, 1, KeyDash);

    def(9, 4, KeySemiColon);
    def(8, 4, KeyComma);
    def(19, 5, KeyPeriod);
    def(11, 3, KeyQuote);

    def(11, 2, KeyLBracket);
    def(12, 2, KeyRBracket);

    def(13, 5, KeyRControl);
    def(12, 4, KeyRShift);
    def(10, 5, KeyRAlt);

    def(0, 5, KeyLControl);
    def(0, 4, KeyLShift);
    def(2, 5, KeyLAlt);

    def(11, 4, KeyBang);

    def(1, 1, KeyNum1);
    def(2, 1, KeyNum2);
    def(3, 1, KeyNum3);
    def(4, 1, KeyNum4);
    def(5, 1, KeyNum5);
    def(6, 1, KeyNum6);
    def(7, 1, KeyNum7);
    def(8, 1, KeyNum8);
    def(9, 1, KeyNum9);
    def(10,1, KeyNum0);

    def(0, 3, KeyMajLock);
    def(14, 0, KeyPrintScreen);
    def(15, 0, KeyScrollLock);
    def(16, 0, KeyPause);
    def(17, 1, KeyNumLock);

    return map;
}
