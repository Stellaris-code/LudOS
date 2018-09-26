/*
keyboard.cpp

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

#include "ps2keyboard.hpp"

#include <kstring/kstring_view.hpp>
#include "utils/kmsgbus.hpp"

#include "i686/interrupts/isr.hpp"
#include "i686/pc/devices/pic.hpp"

#include "drivers/kbd/driver_kbd_event.hpp"
#include "drivers/kbd/led_handler.hpp"
#include "ps2controller.hpp"
#include "io.hpp"

#include "utils/logging.hpp"

enum PS2KeyboardLeds : uint8_t
{
    ScrollLockLed = 0b001,
    NumLockLed = 0b010,
    CapsLockLed = 0b100
};

enum PS2KeyboardCommands : uint8_t
{
    LedCommand = 0xED,
    LedAck = 0xFA,
    KeyboardEnable = 0xAE,
    KeyboardDisable = 0xAD,
    KeyboardResend = 0xFE
};

bool PS2Keyboard::accept()
{
    // FIXME : do
    return true;
}

PS2Keyboard::PS2Keyboard()
{
    isr::register_handler(IRQ1, [this](const registers* const r){return isr(r);});

    init_assocs();

    enable();

    kmsgbus.register_handler<LEDChangeEvent>([this](const LEDChangeEvent& e)
    {
        if (e.caps_led != LEDState::Ignore)
        {
            toggle_led(CapsLockLed, e.caps_led == LEDState::On ? true : false);
        }
        if (e.scroll_led != LEDState::Ignore)
        {
            toggle_led(ScrollLockLed, e.scroll_led == LEDState::On ? true : false);
        }
        if (e.num_led != LEDState::Ignore)
        {
            toggle_led(NumLockLed, e.num_led == LEDState::On ? true : false);
        }
    });

    set_leds(0);

    log(Info, "PS/2 Keyboard driver initialized\n");
}

void PS2Keyboard::enable()
{
    PS2Controller::send_command(KeyboardEnable, false);
    pic::clear_mask(1); // enable keyboard interrupts
}

void PS2Keyboard::disable()
{
    pic::set_mask(1); // disable keyboard interrupts
    PS2Controller::send_command(KeyboardDisable, false);
}

void PS2Keyboard::set_leds(uint8_t leds)
{
    PS2Controller::poll_obf();
    outb(DataPort, LedCommand);
    PS2Controller::poll_obf();
    outb(DataPort, leds);
}

void PS2Keyboard::toggle_led(uint8_t led, bool value)
{
    leds &= ~led;                      // clear
    leds |=  led & (value ? 0xFF : 0); // set to val

    set_leds(leds);
}

kpp::string_view PS2Keyboard::driver_name() const
{
    return "PS/2 Keyboard";
}

bool PS2Keyboard::isr(const registers *)
{
    uint8_t code = inb(0x60);

    auto key_state = DriverKbdEvent::Pressed;

    // TODO : long_key_assocs

    if (code == 0xE0)
    {
        last_is_e0 = true;
    }
    else
    {
        if (code >= 0x80)
        {
            code -= 0x80;
            key_state = DriverKbdEvent::Released;
        }

        // TODO : inherit from keyboard driver and automatically set the kbd id
        if (last_is_e0 && e0_key_assocs[code] != 0xFF)
        {
            kmsgbus.send<DriverKbdEvent>({0, {e0_key_assocs[code]}, key_state});
        }
        else if (key_assocs[code] != 0xFF)
        {
            kmsgbus.send<DriverKbdEvent>({0, {key_assocs[code]}, key_state});
        }

        last_is_e0 = false;
    }

    return true;
}

void PS2Keyboard::init_assocs()
{
    key_assocs.fill(0xFF);
    e0_key_assocs.fill(0xFF);

    define_assoc(0x01, kbd::pos(0, 0), "escape");
    define_assoc(0x02, kbd::pos(1, 1), "1");
    define_assoc(0x03, kbd::pos(2, 1), "2");
    define_assoc(0x04, kbd::pos(3, 1), "3");
    define_assoc(0x05, kbd::pos(4, 1), "4");
    define_assoc(0x06, kbd::pos(5, 1), "5");
    define_assoc(0x07, kbd::pos(6, 1), "6");
    define_assoc(0x08, kbd::pos(7, 1), "7");
    define_assoc(0x09, kbd::pos(8, 1), "8");
    define_assoc(0x0A, kbd::pos(9, 1), "9");
    define_assoc(0x0B, kbd::pos(10, 1),"0");
    define_assoc(0x0C, kbd::pos(11, 1),"-");
    define_assoc(0x0D, kbd::pos(12, 1),"=");
    define_assoc(0x0E, kbd::pos(13, 1),"backspace");
    define_assoc(0x0F, kbd::pos(0, 2), "tab");
    define_assoc(0x10, kbd::pos(1, 2), "q");
    define_assoc(0x11, kbd::pos(2, 2), "w");
    define_assoc(0x12, kbd::pos(3, 2), "e");
    define_assoc(0x13, kbd::pos(4, 2), "r");
    define_assoc(0x14, kbd::pos(5, 2), "t");
    define_assoc(0x15, kbd::pos(6, 2), "y");
    define_assoc(0x16, kbd::pos(7, 2), "u");
    define_assoc(0x17, kbd::pos(8, 2), "i");
    define_assoc(0x18, kbd::pos(9, 2), "o");
    define_assoc(0x19, kbd::pos(10, 2),"p");
    define_assoc(0x1A, kbd::pos(11, 2),"[");
    define_assoc(0x1B, kbd::pos(12, 2),"]");
    define_assoc(0x1C, kbd::pos(12, 3),"enter");
    define_assoc(0x1D, kbd::pos(0, 5), "lctrl");
    define_assoc(0x1E, kbd::pos(1, 3), "a");
    define_assoc(0x1F, kbd::pos(2, 3), "s");
    define_assoc(0x20, kbd::pos(3, 3), "d");
    define_assoc(0x21, kbd::pos(4, 3), "f");
    define_assoc(0x22, kbd::pos(5, 3), "g");
    define_assoc(0x23, kbd::pos(6, 3), "h");
    define_assoc(0x24, kbd::pos(7, 3), "j");
    define_assoc(0x25, kbd::pos(8, 3), "k");
    define_assoc(0x26, kbd::pos(9, 3), "l");
    define_assoc(0x27, kbd::pos(10, 3),";");
    define_assoc(0x28, kbd::pos(11, 3),"'");
    define_assoc(0x29, kbd::pos(0, 1), "`");
    define_assoc(0x2A, kbd::pos(0, 4), "lshift");
    define_assoc(0x2B, kbd::pos(13, 2), "\\");
    define_assoc(0x2C, kbd::pos(2, 4), "z");
    define_assoc(0x2D, kbd::pos(3, 4), "x");
    define_assoc(0x2E, kbd::pos(4, 4), "c");
    define_assoc(0x2F, kbd::pos(5, 4), "v");
    define_assoc(0x30, kbd::pos(6, 4), "b");
    define_assoc(0x31, kbd::pos(7, 4), "n");
    define_assoc(0x32, kbd::pos(8, 4), "m");
    define_assoc(0x33, kbd::pos(9, 4), ",");
    define_assoc(0x34, kbd::pos(10, 4),".");
    define_assoc(0x35, kbd::pos(11, 4),"/");
    define_assoc(0x36, kbd::pos(12, 4),"rshift");
    define_assoc(0x37, kbd::pos(19, 1),"(keypad) *");
    define_assoc(0x38, kbd::pos(2, 5),"lalt");
    define_assoc(0x39, kbd::pos(3, 5),"space");
    define_assoc(0x3A, kbd::pos(0, 3),"capslock");
    define_assoc(0x3B, kbd::pos(1, 0), "F1");
    define_assoc(0x3C, kbd::pos(2, 0), "F2");
    define_assoc(0x3D, kbd::pos(3, 0), "F3");
    define_assoc(0x3E, kbd::pos(4, 0), "F4");
    define_assoc(0x3F, kbd::pos(5, 0), "F5");
    define_assoc(0x40, kbd::pos(6, 0), "F6");
    define_assoc(0x41, kbd::pos(7, 0), "F7");
    define_assoc(0x42, kbd::pos(8, 0), "F8");
    define_assoc(0x43, kbd::pos(9, 0), "F9");
    define_assoc(0x44, kbd::pos(10, 0),"F10");
    define_assoc(0x45, kbd::pos(17, 1),"numlock");
    define_assoc(0x46, kbd::pos(15, 0),"scrolllock");
    define_assoc(0x47, kbd::pos(17, 2),"(keypad) 7");
    define_assoc(0x48, kbd::pos(18, 2),"(keypad) 8");
    define_assoc(0x49, kbd::pos(19, 2),"(keypad) 9");
    define_assoc(0x4A, kbd::pos(20, 1),"(keypad) -");
    define_assoc(0x4B, kbd::pos(17, 3),"(keypad) 4");
    define_assoc(0x4C, kbd::pos(18, 3),"(keypad) 5");
    define_assoc(0x4D, kbd::pos(19, 3),"(keypad) 6");
    define_assoc(0x4E, kbd::pos(20, 3),"(keypad) +");
    define_assoc(0x4F, kbd::pos(17, 4),"(keypad) 1");
    define_assoc(0x50, kbd::pos(18, 4),"(keypad) 2");
    define_assoc(0x51, kbd::pos(19, 4),"(keypad) 3");
    define_assoc(0x52, kbd::pos(17, 5),"(keypad) 0");
    define_assoc(0x53, kbd::pos(19, 5),"(keypad) .");
    define_assoc(0x57, kbd::pos(11, 0),"F11");
    define_assoc(0x58, kbd::pos(12, 0),"F12");

    define_e0_assoc(0x1C, kbd::pos(20, 5), "(keypad) enter");
    define_e0_assoc(0x1D, kbd::pos(13, 5), "rctrl");
    define_e0_assoc(0x35, kbd::pos(18, 1), "(keypad) /");
    define_e0_assoc(0x38, kbd::pos(10, 5), "ralt");
    define_e0_assoc(0x47, kbd::pos(15, 1), "home");
    define_e0_assoc(0x48, kbd::pos(15, 4), "cursor up");
    define_e0_assoc(0x49, kbd::pos(16, 1), "page up");
    define_e0_assoc(0x4B, kbd::pos(14, 5), "cursor left");
    define_e0_assoc(0x4D, kbd::pos(16, 5), "cursor right");
    define_e0_assoc(0x4F, kbd::pos(15, 2), "end");
    define_e0_assoc(0x50, kbd::pos(15, 5), "cursor down");
    define_e0_assoc(0x51, kbd::pos(16, 2), "page down");
    define_e0_assoc(0x52, kbd::pos(14, 1), "insert");
    define_e0_assoc(0x53, kbd::pos(14, 2), "delete");

    long_key_assocs.emplace_back(kbd::pos(14, 0), std::vector<uint8_t>{0xE0, 0x2A, 0xE0, 0xAA});
    long_key_assocs.emplace_back(kbd::pos(16, 0), std::vector<uint8_t>{0xE1, 0x1D, 0x45, 0xE1, 0x9D, 0xC5});
}

void PS2Keyboard::define_assoc(uint8_t i, uint8_t pos, const kpp::string &name)
{
    key_assocs[i] = pos;
}

void PS2Keyboard::define_e0_assoc(uint8_t i, uint8_t pos, const kpp::string &name)
{
    e0_key_assocs[i] = pos;
}

ADD_DRIVER(PS2Keyboard)
