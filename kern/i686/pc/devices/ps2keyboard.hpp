/*
keyboard.hpp

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
#ifndef PS2KEYBOARD_HPP
#define PS2KEYBOARD_HPP

#include "i686/pc/cpu/registers.hpp"

#include <array.hpp>
#include <vector.hpp>
#include <string.hpp>

#define SCROLL_LOCK_LED 0b001
#define NUM_LOCK_LED 0b010
#define CAPS_LOCK_LED   0b100
#define COMMAND_PORT 0x64
#define KBD_PORT 0x60
#define LED_CMD 0xED
#define LED_ACK 0xFA
#define KBD_ENABLE 0xAE
#define KBD_DISABLE 0xAD
#define KBD_RESEND 0xFE

class PS2Keyboard
{
public:
    static void init();

    static void enable();
    static void disable();

    static void set_leds(uint8_t leds);
    static void toggle_led(uint8_t led, bool value);

private:
    static void isr(const registers*);

    static void send_command(uint8_t command, bool poll = true);

    static void poll_ibf();
    static void poll_obf();

    static void init_assocs();

    static void define_assoc(uint8_t i, uint8_t pos, const std::string& name);
    static void define_e0_assoc(uint8_t i, uint8_t pos, const std::string& name);

private:
    static inline uint8_t leds { 0 };

    static inline bool last_is_e0 { false };

    static inline std::array<uint8_t, 256> key_assocs;

    static inline std::array<uint8_t, 256> e0_key_assocs;

    static inline std::vector<std::pair<uint8_t, std::vector<uint8_t>>> long_key_assocs;
};

#endif // KEYBOARD_HPP
