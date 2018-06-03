/*
powermanagement.cpp

Copyright (c) 03 Yann BOUCHER (yann)

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

#include "power/powermanagement.hpp"

#include "io.hpp"
#include "halt.hpp"
#include "terminal/terminal.hpp"
#include "utils/env.hpp"
#include "utils/defs.hpp"
#include "utils/logging.hpp"
#include "utils/messagebus.hpp"

#ifdef USES_ACPI
#include "drivers/acpi/powermanagement.hpp"
#endif

namespace power
{

inline bool kbd_reset()
{
    uint8_t good = 0x02;
    while (good & 0x02)
        good = inb(0x64);
    outb(0x64, 0xFE);

    // if we are still on after that, there is definitly a problem, return false

    return false;
}

[[noreturn]]
void reset()
{
    if (!kbd_reset())
    {
        err("Cannot reset computer\n");
        err("Manual Reset required\n");

        halt();
    }
    unreachable();
}

//[[noreturn]]
void shutdown()
{
    ksetenv("TERM_BCKG", "");

    term().enable();
    term().scroll_bottom();
    term().draw();

    term().set_scrolling(false);

    term_data().push_color({0xffffff, 0x0000ff});
    term_data().clear();

    term().clear({0xffffff, 0x0000ff});

    const char message[] = "You can now safely shutdown your computer.";

    for (size_t i { 0 }; i < term().height()/2-1; ++i)
    {
        putchar('\n');
    }
    for (size_t i { 0 }; i < term().width()/2-sizeof(message)/2; ++i)
    {
        putchar(' ');
    }
    kprintf(message);

    term().force_redraw();

    halt();
}

void init_power_management()
{
#ifdef USES_ACPI
    acpi::power::init();
#else
    MessageBus::register_handler<ResetMessage>([](const ResetMessage&){reset();}, MessageBus::Last);
    MessageBus::register_handler<ShutdownMessage>([](const ShutdownMessage&){shutdown();}, MessageBus::Last);

#endif

}
}
