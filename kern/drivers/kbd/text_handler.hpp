/*
text_handler.hpp

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

#ifndef TEXT_HANDLER
#define TEXT_HANDLER

#include <stdint.h>

#include "kbd_mappings.hpp"

class Keyboard;

namespace kbd
{

struct TextEnteredEvent
{
    char32_t c;
};

class TextHandler
{
    friend class ::Keyboard;

public:
    static void init();

private:
    static void handle_key(const kbd::KeyEvent& e);

    static void handle_modifiers(const kbd::KeyEvent& e);

private:
    static inline bool lshift { false };
    static inline bool rshift { false };
    static inline bool lalt { false };
    static inline bool ralt { false };
    static inline bool lctrl { false };
    static inline bool rctrl { false };
    static inline bool capslock { false };
    static inline bool alt { false };
    static inline bool numlock { false };
};

}

#endif
