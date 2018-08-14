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

#include "utils/messagebus.hpp"

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

        if (e.key >= KeyA && e.key <= KeyZ)
        {
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>(e.key - KeyA + 'A' + (caps ? 0 : 0x20))});
            return;
        }
        if (e.key >= KeyNum0 && e.key <= KeyNum9 && caps)
        {
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>(e.key - KeyNum0 + '0')});
            return;
        }
        if (e.key >= KeyNumpad0 && e.key <= KeyNumpad9 && numlock)
        {
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>(e.key - KeyNumpad0 + '0')});
            return;
        }

        switch (e.key)
        {
        case KeyLBracket:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>('^')});
            return;
        case KeyRBracket:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>('$')});
            return;
        case KeySemiColon:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>(caps ? '.' : ';')});
            return;
        case KeyComma:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>(caps ? '?' : ',')});
            return;
        case KeyBang:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>('!')});
            return;
        case KeyQuote:
            if (caps)
            {
                MessageBus::send<TextEnteredEvent>({static_cast<char32_t>('%')});
                return;
            }
            else
                break;
        case KeyDash:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>(alt ? ']' : ')')});
            return;
        case KeyEqual:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>(caps ? '+' : alt ? '}' : '=')});
            return;
        case KeyTilde:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>(caps ? '~' : U'é')});
            return;
        case KeyBackSlash:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>('*')});
            return;
        case KeySlash:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>(caps ? '/' : ':')});
            return;
        case KeyTab:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>('\t')});
            return;
        case KeyBackSpace:
        //case KeyDelete:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>('\b')});
            return;
        case KeyReturn:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>('\n')});
            return;
        case KeySpace:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>(' ')});
            return;

        case KeyAdd:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>('+')});
            return;
        case KeySubtract:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>('-')});
            return;
        case KeyMultiply:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>('*')});
            return;
        case KeyDivide:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>('/')});
            return;

        case KeyNum0:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>(alt ? '@' : U'à')});
            return;
        case KeyNum1:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>('&')});
            return;
        case KeyNum2:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>(alt ? '~' : U'é')});
            return;
        case KeyNum3:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>(alt ? '#' : '"')});
            return;
        case KeyNum4:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>(alt ? '{' : '\'')});
            return;
        case KeyNum5:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>(alt ? '[' : '(')});
            return;
        case KeyNum6:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>(alt ? '|' : '-')});
            return;
        case KeyNum7:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>(alt ? '`' : U'è')});
            return;
        case KeyNum8:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>(alt ? '\\' : '_')});
            return;
        case KeyNum9:
            MessageBus::send<TextEnteredEvent>({static_cast<char32_t>(alt ? '^' : U'ç')});
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
