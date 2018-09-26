/*
text_handler.cpp

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

#include "text_handler.hpp"

#include "utils/kmsgbus.hpp"

void kbd::TextHandler::init()
{
    kmsgbus.register_handler<kbd::KeyEvent>(&handle_key);
}

void kbd::TextHandler::handle_key(const kbd::KeyEvent &e)
{
    handle_modifiers(e);

    bool caps = ((lshift || rshift) ^ capslock);
    bool alt = lalt || ralt;

    if (e.state == KeyEvent::Pressed)
    {

        if (e.key >= KeyA && e.key <= KeyZ)
        {
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>(e.key - KeyA + 'A' + (caps ? 0 : 0x20))});
            return;
        }
        if (e.key >= KeyNum0 && e.key <= KeyNum9 && caps)
        {
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>(e.key - KeyNum0 + '0')});
            return;
        }
        if (e.key >= KeyNumpad0 && e.key <= KeyNumpad9 && numlock)
        {
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>(e.key - KeyNumpad0 + '0')});
            return;
        }

        switch (e.key)
        {
        case KeyLBracket:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>('^')});
            return;
        case KeyRBracket:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>('$')});
            return;
        case KeySemiColon:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>(caps ? '.' : ';')});
            return;
        case KeyComma:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>(caps ? '?' : ',')});
            return;
        case KeyBang:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>('!')});
            return;
        case KeyQuote:
            if (caps)
            {
                kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>('%')});
                return;
            }
            else
                break;
        case KeyDash:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>(alt ? ']' : ')')});
            return;
        case KeyEqual:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>(caps ? '+' : alt ? '}' : '=')});
            return;
        case KeyTilde:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>(caps ? '~' : U'é')});
            return;
        case KeyBackSlash:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>('*')});
            return;
        case KeySlash:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>(caps ? '/' : ':')});
            return;
        case KeyTab:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>('\t')});
            return;
        case KeyBackSpace:
        //case KeyDelete:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>('\b')});
            return;
        case KeyReturn:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>('\n')});
            return;
        case KeySpace:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>(' ')});
            return;

        case KeyAdd:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>('+')});
            return;
        case KeySubtract:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>('-')});
            return;
        case KeyMultiply:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>('*')});
            return;
        case KeyDivide:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>('/')});
            return;

        case KeyNum0:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>(alt ? '@' : U'à')});
            return;
        case KeyNum1:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>('&')});
            return;
        case KeyNum2:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>(alt ? '~' : U'é')});
            return;
        case KeyNum3:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>(alt ? '#' : '"')});
            return;
        case KeyNum4:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>(alt ? '{' : '\'')});
            return;
        case KeyNum5:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>(alt ? '[' : '(')});
            return;
        case KeyNum6:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>(alt ? '|' : '-')});
            return;
        case KeyNum7:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>(alt ? '`' : U'è')});
            return;
        case KeyNum8:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>(alt ? '\\' : '_')});
            return;
        case KeyNum9:
            kmsgbus.send<TextEnteredEvent>({static_cast<char32_t>(alt ? '^' : U'ç')});
            return;

        default:
            return;
        }
    }
}

void kbd::TextHandler::handle_modifiers(const kbd::KeyEvent &e)
{
    if (e.key == KeyMajLock && e.state == kbd::KeyEvent::Pressed)
    {
        capslock = !capslock;
    }
    else if (e.key == KeyLShift)
    {
        lshift = e.state == kbd::KeyEvent::Pressed;
    }
    else if (e.key == KeyRShift)
    {
        rshift = e.state == kbd::KeyEvent::Pressed;
    }
    else if (e.key == KeyNumLock && e.state == kbd::KeyEvent::Pressed)
    {
        numlock = !numlock;
    }
    else if (e.key == KeyLAlt)
    {
        lalt = e.state == kbd::KeyEvent::Pressed;
    }
    else if (e.key == KeyRAlt)
    {
        ralt = e.state == kbd::KeyEvent::Pressed;
    }
    else if (e.key == KeyLControl)
    {
        lctrl = e.state == kbd::KeyEvent::Pressed;
    }
    else if (e.key == KeyRControl)
    {
        rctrl = e.state == kbd::KeyEvent::Pressed;
    }
}
