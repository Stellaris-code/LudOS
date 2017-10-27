/*
powermanagement.hpp

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
#ifndef I686_POWERMANAGEMENT_HPP
#define I686_POWERMANAGEMENT_HPP

#include "acpi.h"

#include "halt.hpp"

#include "utils/logging.hpp"
#include "utils/builtins.hpp"

#include "power/powermanagement.hpp"

#include "io.hpp"

namespace acpi::power
{

inline bool acpi_reset()
{
    return ACPI_SUCCESS(AcpiReset());
}

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
inline void reset()
{
    if (!acpi_reset() && ! kbd_reset())
    {
        err("Cannot reset computer\n");
        err("Manual Reset required\n");

        halt();
    }
    unreachable();
}

[[noreturn]]
inline void shutdown()
{
    ACPI_STATUS status = AcpiEnterSleepStatePrep(0x5);
    if (ACPI_FAILURE(status))
    {
        err("Sleep prep failure : Status : '%s'\n", AcpiFormatException(status));
        err("Manual shutdown required\n");
        halt();
    }
    else
    {
        cli(); // disable interrupts
        status = AcpiEnterSleepState(0x5);
        if (ACPI_FAILURE(status))
        {
            err("Sleep failure : Status : '%s'\n", AcpiFormatException(status));
            err("Manual shutdown required\n");
            halt();
        }

        halt(); // should not reach that point
    }
}


inline bool init()
{
    ::reset = power::reset;
    ::shutdown = power::shutdown;
}

}

#endif // POWERMANAGEMENT_HPP
