/*
acpitimer.hpp

Copyright (c) 17 Yann BOUCHER (yann)

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
#ifndef ACPITIMER_HPP
#define ACPITIMER_HPP

#include <stdint.h>

#include "acpi.h"

#include "utils/nop.hpp"

class ACPITimer
{
public:
    static inline UINT32 ticks()
    {
        UINT32 ticks;
        AcpiGetTimer(&ticks);

        return ticks;
    }

    static inline UINT32 elapsed_time(UINT32 start, UINT32 end)
    {
        UINT32 duration;
        AcpiGetTimerDuration(start, end, &duration);

        return duration;
    }

    // ms
    static inline void sleep(UINT32 time)
    {
        UINT32 start = ticks();
        while (elapsed_time(start, ticks()) < time*1000) { nop(); }

        return;
    }

    static inline bool active { false };
};

#endif // ACPITIMER_HPP
