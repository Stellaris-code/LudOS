/*
led_handler.hpp

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
#ifndef LED_HANDLER_HPP
#define LED_HANDLER_HPP

#include "kbd_mappings.hpp"
#include "utils/messagebus.hpp"

enum class LEDState : uint8_t
{
    Ignore,
    Off,
    On
};

struct LEDChangeEvent
{
    size_t kbd_id;
    LEDState num_led { LEDState::Ignore };
    LEDState caps_led { LEDState::Ignore };
    LEDState scroll_led { LEDState::Ignore };
};

namespace kbd
{
inline void install_led_handler()
{
    MessageBus::register_handler<KeyEvent>([](const KeyEvent& e)
    {
        static bool caps_led[max_kbd_count] { false };
        static bool num_led[max_kbd_count] { false };
        static bool scroll_led[max_kbd_count] { false };

        LEDChangeEvent leds;
        leds.kbd_id = e.kbd_id;

        if (e.state == KeyEvent::Pressed)
        {

            switch (e.key)
            {
            case KeyMajLock:
                caps_led[e.kbd_id] = !caps_led[e.kbd_id];
                leds.caps_led = caps_led[e.kbd_id] ? LEDState::On : LEDState::Off;
                break;
            case KeyNumLock:
                num_led[e.kbd_id] = !num_led[e.kbd_id];
                leds.num_led = num_led[e.kbd_id] ? LEDState::On : LEDState::Off;
                break;
            case KeyScrollLock:
                scroll_led[e.kbd_id] = !scroll_led[e.kbd_id];
                leds.scroll_led = scroll_led[e.kbd_id] ? LEDState::On : LEDState::Off;
                break;
            }

            MessageBus::send(leds);

        }
    });
}
}

#endif // LED_HANDLER_HPP
