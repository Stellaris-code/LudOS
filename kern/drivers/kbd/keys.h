/*
keys.h

Copyright (c) 14 Yann BOUCHER (yann)

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
#ifndef KEYS_H
#define KEYS_H

#include <stdint.h>

enum Key : uint8_t
{
    KeyUnknown = 0,  ///< Unhandled key
    KeyA = 1,      ///< The A key
    KeyB,      ///< The B key
    KeyC,      ///< The C key
    KeyD,      ///< The D key
    KeyE,      ///< The E key
    KeyF,      ///< The F key
    KeyG,      ///< The G key
    KeyH,      ///< The H key
    KeyI,      ///< The I key
    KeyJ,      ///< The J key
    KeyK,      ///< The K key
    KeyL,      ///< The L key
    KeyM,      ///< The M key
    KeyN,      ///< The N key
    KeyO,      ///< The O key
    KeyP,      ///< The P key
    KeyQ,      ///< The Q key
    KeyR,      ///< The R key
    KeyS,      ///< The S key
    KeyT,      ///< The T key
    KeyU,      ///< The U key
    KeyV,      ///< The V key
    KeyW,      ///< The W key
    KeyX,      ///< The X key
    KeyY,      ///< The Y key
    KeyZ,      ///< The Z key

    KeyNum0,       ///< The 0 key
    KeyNum1,       ///< The 1 key
    KeyNum2,       ///< The 2 key
    KeyNum3,       ///< The 3 key
    KeyNum4,       ///< The 4 key
    KeyNum5,       ///< The 5 key
    KeyNum6,       ///< The 6 key
    KeyNum7,       ///< The 7 key
    KeyNum8,       ///< The 8 key
    KeyNum9,       ///< The 9 key

    KeyEscape,      ///< The Escape key

    KeyLControl,    ///< The left Control key
    KeyLShift,      ///< The left Shift key
    KeyLAlt,       ///< The left Alt key
    KeyLSystem,     ///< The left OS specific key: window (Windows and Linux), apple (MacOS X), ...

    KeyRControl,    ///< The right Control key
    KeyRShift,      ///< The right Shift key
    KeyRAlt,       ///< The right Alt key
    KeyRSystem,     ///< The right OS specific key: window (Windows and Linux), apple (MacOS X), ...

    KeyMenu,       ///< The Menu key

    KeyLBracket,    ///< The [ key
    KeyRBracket,    ///< The ] key

    KeySemiColon,   ///< The ; key
    KeyComma,      ///< The , key
    KeyPeriod,      ///< The . key
    KeyQuote,      ///< The ' key

    KeySlash,      ///< The / key
    KeyBackSlash,   ///< The \ key
    KeyTilde,      ///< The ~ key
    KeyEqual,      ///< The = key
    KeyDash,       ///< The - key

    KeySpace,      ///< The Space key
    KeyReturn,      ///< The Return key
    KeyBackSpace,   ///< The Backspace key
    KeyTab,        ///< The Tabulation key

    KeyPageUp,      ///< The Page up key
    KeyPageDown,    ///< The Page down key
    KeyEnd,        ///< The End key
    KeyHome,       ///< The Home key
    KeyInsert,      ///< The Insert key
    KeyDelete,      ///< The Delete key

    KeyAdd,        ///< The + key
    KeySubtract,    ///< The - key
    KeyMultiply,    ///< The * key
    KeyDivide,      ///< The / key
    KeyBang,       ///< The ! key

    KeyLeft,       ///< Left arrow
    KeyRight,      ///< Right arrow
    KeyUp,         ///< Up arrow
    KeyDown,       ///< Down arrow
    KeyNumpad0,     ///< The numpad 0 key
    KeyNumpad1,     ///< The numpad 1 key
    KeyNumpad2,     ///< The numpad 2 key
    KeyNumpad3,     ///< The numpad 3 key
    KeyNumpad4,     ///< The numpad 4 key
    KeyNumpad5,     ///< The numpad 5 key
    KeyNumpad6,     ///< The numpad 6 key
    KeyNumpad7,     ///< The numpad 7 key
    KeyNumpad8,     ///< The numpad 8 key
    KeyNumpad9,     ///< The numpad 9 key
    KeyF1,         ///< The F1 key
    KeyF2,         ///< The F2 key
    KeyF3,         ///< The F3 key
    KeyF4,         ///< The F4 key
    KeyF5,         ///< The F5 key
    KeyF6,         ///< The F6 key
    KeyF7,         ///< The F7 key
    KeyF8,         ///< The F8 key
    KeyF9,         ///< The F9 key
    KeyF10,        ///< The F10 key
    KeyF11,        ///< The F11 key
    KeyF12,        ///< The F12 key
    KeyF13,        ///< The F13 key
    KeyF14,        ///< The F14 key
    KeyF15,        ///< The F15 key
    KeyPause,      ///< The Pause key

    KeyMajLock,
    KeyScrollLock,
    KeyPrintScreen,
    KeyNumLock,

    KeyCount     ///< Keep last -- the total number of keyboard keys
};

#endif // KEYS_H
