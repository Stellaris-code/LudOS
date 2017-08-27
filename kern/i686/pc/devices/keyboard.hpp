/*
keyboard.hpp

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
#ifndef KEYBOARD_HPP
#define KEYBOARD_HPP

#include "utils/stdint.h"
#include "../registers.hpp"

#define SCROLL_LOCK_LED 0b001
#define NUM_LOCK_LED 0b010
#define CAPS_LOCK_LED   0b100
#define KBD_PORT 0x60
#define LED_CMD 0xED
#define LED_ACK 0xFA

class Keyboard
{
public:
    struct Event
    {
        bool shift : 1;
        bool alt : 1;
        bool ctrl : 1;
        bool fn : 1;

        bool caps : 1;
        bool scroll : 1;
        bool num : 1;

        bool state : 1;

        // Mapped Key Code
        uint8_t keycode;
    };

public:
    static void init();

    static void set_leds(uint8_t leds);
    static void toggle_led(uint8_t led, bool value = true);

private:
    static void isr(const registers* const);
    static void wait();

public:
    static inline void (*handle_char)(char); // callback
    static inline void (*kbd_event)(Event);

    static inline bool lshift { false };
    static inline bool rshift { false };
    static inline bool ctrl   { false };
    static inline bool alt    { false };

    static inline bool caps_lock { false };
    static inline bool scroll_lock { false };
    static inline bool num_lock { false };

    static inline uint8_t leds { 0b000 };

private:
    static constexpr uint8_t kbdmap[] = {
            0x1B, 0x1B, 0x1B, 0x1B,	/*      esc     (0x01)  */
            '1', '!', '1', '1',
            '2', '@', '2', '2',
            '3', '#', '3', '3',
            '4', '$', '4', '4',
            '5', '%', '5', '5',
            '6', '^', '6', '6',
            '7', '&', '7', '7',
            '8', '*', '8', '8',
            '9', '(', '9', '9',
            '0', ')', '0', '0',
            '-', '_', '-', '-',
            '=', '+', '=', '=',
            0x08, 0x08, 0x7F, 0x08,	/*      backspace       */
            0x09, 0x09, 0x09, 0x09,	/*      tab     */
            'q', 'Q', 'q', 'q',
            'w', 'W', 'w', 'w',
            'e', 'E', 'e', 'e',
            'r', 'R', 'r', 'r',
            't', 'T', 't', 't',
            'y', 'Y', 'y', 'y',
            'u', 'U', 'u', 'u',
            'i', 'I', 'i', 'i',
            'o', 'O', 'o', 'o',
            'p', 'P', 'p', 'p',
            '[', '{', '[', '[',
            ']', '}', ']', ']',
            0x0A, 0x0A, 0x0A, 0x0A,	/*      enter   */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      ctrl    */
            'a', 'A', 'a', 'a',
            's', 'S', 's', 's',
            'd', 'D', 'd', 'd',
            'f', 'F', 'f', 'f',
            'g', 'G', 'g', 'g',
            'h', 'H', 'h', 'h',
            'j', 'J', 'j', 'j',
            'k', 'K', 'k', 'k',
            'l', 'L', 'l', 'l',
            ';', ':', ';', ';',
            0x27, 0x22, 0x27, 0x27,	/*      '"      */
            '`', '~', '`', '`',	/*      `~      */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      Lshift  (0x2a)  */
            '\\', '|', '\\', '\\',
            'z', 'Z', 'z', 'z',
            'x', 'X', 'x', 'x',
            'c', 'C', 'c', 'c',
            'v', 'V', 'v', 'v',
            'b', 'B', 'b', 'b',
            'n', 'N', 'n', 'n',
            'm', 'M', 'm', 'm',
            0x2C, 0x3C, 0x2C, 0x2C,	/*      ,<      */
            0x2E, 0x3E, 0x2E, 0x2E,	/*      .>      */
            0x2F, 0x3F, 0x2F, 0x2F,	/*      /?      */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      Rshift  (0x36)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x37)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x38)  */
            ' ', ' ', ' ', ' ',	/*      space   */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x3a)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x3b)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x3c)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x3d)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x3e)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x3f)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x40)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x41)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x42)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x43)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x44)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x45)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x46)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x47)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x48)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x49)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x4a)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x4b)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x4c)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x4d)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x4e)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x4f)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x50)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x51)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x52)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x53)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x54)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x55)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x56)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x57)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x58)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x59)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x5a)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x5b)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x5c)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x5d)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x5e)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x5f)  */
            0xFF, 0xFF, 0xFF, 0xFF,	/*      (0x60)  */
            0xFF, 0xFF, 0xFF, 0xFF	/*      (0x61)  */
    };
};

#endif // KEYBOARD_HPP
