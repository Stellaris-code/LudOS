/*
keyboard.cpp

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

#include "keyboard.hpp"

#include "io.hpp"
#include "../isr.hpp"
#include "utils/logging.hpp"
#include <stdio.h>
#include <ctype.h>
#include <atoi.h>

void Keyboard::init()
{
    isr::register_handler(IRQ1, &Keyboard::isr);

    handlers[0x48] = [](const Event&)
    {
        //kprintf("Hey\n");
        wait();
        uint8_t code = inb(KBD_PORT);
        if (code == 0xE0)
        {
            Terminal::show_history(Terminal::current_history()+10);
            return false;
        }
        return true;
    };
    handlers[0x50] = [](const Event&)
    {
        wait();
        uint8_t code = inb(KBD_PORT);
        if (code == 0xE0)
        {
            Terminal::show_history(Terminal::current_history()-10);
            return false;
        }
        return true;
    };

    log("Keyboard initialized\n");
}

void Keyboard::set_leds(uint8_t leds)
{
    outb(KBD_PORT, LED_CMD);
    wait();
    outb(KBD_PORT, leds);
    wait();
}

void Keyboard::toggle_led(uint8_t led, bool value)
{
    leds &= ~led;         // clear
    leds |=  led & (value ? 0xFF : 0); // set to val

    set_leds(leds);
}

void Keyboard::set_kbdmap(const uint8_t *map)
{
    kbdmap = map;
}

void Keyboard::isr(const registers * const)
{
    uint8_t key;
    do
    {
        key = inb(0x64);
    } while ((key & 0x01) == 0);

    key = inb(KBD_PORT);
    key--;

    uint8_t unaltered_key = key;


    bool pressed = false;

    switch (key)
    {
    case 0x45:
        scroll_lock = !scroll_lock;
        toggle_led(SCROLL_LOCK_LED, scroll_lock);
        break;
    case 0x44:
        num_lock = !num_lock;
        toggle_led(NUM_LOCK_LED, num_lock);
        break;
    case 0x39:
        caps_lock = !caps_lock;
        toggle_led(CAPS_LOCK_LED, caps_lock);
        break;
    }

    bool is_handle_char { false };

    if (key < 0x80)
    {         /* touche enfoncee */
        pressed = true;
        switch (key) {
        case 0x29:
            lshift = true;
            break;
        case 0x35:
            rshift = true;
            break;
        case 0x1C:
            ctrl = true;
            break;
        case 0x37:
            alt = true;
            break;
        default:
            is_handle_char = true;
        }
    }
    else
    {                /* touche relachee */
        key -= 0x80;
        switch (key)
        {
        case 0x29:
            lshift = false;
            break;
        case 0x35:
            rshift = false;
            break;
        case 0x1C:
            ctrl = false;
            break;
        case 0x37:
            alt = false;
            break;
        }
    }

    Event event {(lshift || rshift), alt, ctrl, false, caps_lock, scroll_lock, num_lock, pressed, unaltered_key};
    if ((!handlers[unaltered_key] || handlers[unaltered_key](event)) && is_handle_char)
    {
        handle_char(kbdmap
                    [key * 4 + (((lshift || rshift) ^ caps_lock) ? 1 : alt ? 2 : num_lock ? 3 : 0)]);
    }
                Terminal::push_color(0);Terminal::pop_color();
    if (kbd_event)
    {
        kbd_event(event);
    }
}

void Keyboard::wait()
{
    uint8_t key;
    do
    {
        key = inb(0x64);
    } while ((key & 0x01) == 0);
}
