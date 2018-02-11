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

#include "i686/cpu/registers.hpp"

#include <array.hpp>
#include <vector.hpp>
#include <string.hpp>

#include "drivers/driver.hpp"

class PS2Keyboard : public Driver
{
public:
    PS2Keyboard();

public:
    static bool accept();

    void enable();
    void disable();

    void set_leds(uint8_t leds);
    void toggle_led(uint8_t led, bool value);

    virtual std::string driver_name() const override { return "PS/2 Keyboard"; }

private:
    bool isr(const registers*);

    void init_assocs();

    void define_assoc(uint8_t i, uint8_t pos, const std::string& name);
    void define_e0_assoc(uint8_t i, uint8_t pos, const std::string& name);

private:
    uint8_t leds { 0 };

    bool last_is_e0 { false };

    std::array<uint8_t, 256> key_assocs;

    std::array<uint8_t, 256> e0_key_assocs;

    std::vector<std::pair<uint8_t, std::vector<uint8_t>>> long_key_assocs;
};

#endif // KEYBOARD_HPP
