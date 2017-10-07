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
        static bool caps_led { false };
        static bool num_led { false };
        static bool scroll_led { false };

        LEDChangeEvent leds;

        if (e.state == KeyEvent::Pressed)
        {

            switch (e.key)
            {
            case kbd::MajLock:
                caps_led = !caps_led;
                leds.caps_led = caps_led ? LEDState::On : LEDState::Off;
                break;
            case kbd::NumLock:
                num_led = !num_led;
                leds.num_led = num_led ? LEDState::On : LEDState::Off;
                break;
            case kbd::ScrollLock:
                scroll_led = !scroll_led;
                leds.scroll_led = scroll_led ? LEDState::On : LEDState::Off;
                break;
            }

            MessageBus::send(leds);

        }
    });
}
}

#endif // LED_HANDLER_HPP
