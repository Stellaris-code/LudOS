/*
cpuinfo.hpp

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
#ifndef CPUINFO_HPP
#define CPUINFO_HPP

#include "timestamp.hpp"
#include "nop.hpp"
#include "timer.hpp"

// in Mhz
inline uint64_t clock_speed(bool recompute = false)
{
    static uint64_t speed;
    static bool computed = false;
    if (computed && !recompute)
    {
        return speed;
    }

    const uint32_t step { 10 };

    uint64_t current = rdtsc();
    uint32_t ticks = Timer::ticks();
    while (ticks + step > Timer::ticks()) { nop(); } // wait 'til a tick is elapsed

    uint64_t elapsed = rdtsc() - current;
    speed = elapsed*Timer::freq()/1'000'000/step;
    computed = true;
    return speed; // MHz
}

#endif // CPUINFO_HPP
