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

#include "messagebus.hpp"

void kbd::TextHandler::init()
{
    MessageBus::register_handler<kbd::KeyEvent>(&handle_key);
}

void kbd::TextHandler::handle_key(const kbd::KeyEvent &e)
{
    handle_modifiers(e);

    bool caps = ((lshift || rshift) ^ capslock);
    bool alt = lalt || ralt;

    if (e.state == KeyEvent::Pressed)
    {

        if (e.key >= A && e.key <= Z)
        {
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>(e.key - A + 'A' + (caps ? 0 : 0x20))});
            return;
        }
        if (e.key >= Num0 && e.key <= Num9 && caps)
        {
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>(e.key - Num0 + '0')});
            return;
        }
        if (e.key >= Numpad0 && e.key <= Numpad9 && numlock)
        {
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>(e.key - Numpad0 + '0')});
            return;
        }

        switch (e.key)
        {
        case LBracket:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>('^')});
            return;
        case RBracket:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>('$')});
            return;
        case SemiColon:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>(caps ? '.' : ';')});
            return;
        case Comma:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>(caps ? '?' : ',')});
            return;
        case Bang:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>('!')});
            return;
        case Quote:
            if (caps)
            {
                MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>('%')});
                return;
            }
            else
                break;
        case Dash:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>(alt ? ']' : ')')});
            return;
        case Equal:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>(caps ? '+' : alt ? '}' : '=')});
            return;
        case Tilde:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>(caps ? '~' : 0xFD)});
            return;
        case BackSlash:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>('*')});
            return;
        case Slash:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>(caps ? '/' : ':')});
            return;
        case Tab:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>('\t')});
            return;
        case BackSpace:
        case Delete:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>('\b')});
            return;
        case Return:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>('\n')});
            return;
        case Space:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>(' ')});
            return;

        case Add:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>('+')});
            return;
        case Subtract:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>('-')});
            return;
        case Multiply:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>('*')});
            return;
        case Divide:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>('/')});
            return;

        case Num0:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>(alt ? '@' : 0x85)});
            return;
        case Num1:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>('&')});
            return;
        case Num2:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>(alt ? '~' : 0x82)});
            return;
        case Num3:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>(alt ? '#' : '"')});
            return;
        case Num4:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>(alt ? '{' : '\'')});
            return;
        case Num5:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>(alt ? '[' : '(')});
            return;
        case Num6:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>(alt ? '|' : '-')});
            return;
        case Num7:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>(alt ? '`' : 0x88)});
            return;
        case Num8:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>(alt ? '\\' : '_')});
            return;
        case Num9:
            MessageBus::send<TextEnteredEvent>({static_cast<uint8_t>(alt ? '^' : 0x87)});
            return;

        default:
            return;
        }
    }
}

void kbd::TextHandler::handle_modifiers(const kbd::KeyEvent &e)
{
    if (e.key == MajLock && e.state == kbd::KeyEvent::Pressed)
    {
        capslock = !capslock;
    }
    else if (e.key == LShift)
    {
        lshift = e.state == kbd::KeyEvent::Pressed;
    }
    else if (e.key == RShift)
    {
        rshift = e.state == kbd::KeyEvent::Pressed;
    }
    else if (e.key == NumLock && e.state == kbd::KeyEvent::Pressed)
    {
        numlock = !numlock;
    }
    else if (e.key == LAlt)
    {
        lalt = e.state == kbd::KeyEvent::Pressed;
    }
    else if (e.key == RAlt)
    {
        ralt = e.state == kbd::KeyEvent::Pressed;
    }
}
