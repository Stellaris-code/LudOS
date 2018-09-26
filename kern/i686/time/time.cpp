/*
timestamp.cpp

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

#include "time/time.hpp"
#include "time/timer.hpp"

#include "i686/cpu/cpuinfo.hpp"
#include "i686/pc/devices/rtc.hpp"

namespace Time
{

bool timer_ready = false;

uint64_t clock_speed()
{
    return ::clock_speed();
}

uint64_t total_ticks()
{
    uint64_t ret;
    asm volatile ( "rdtsc" : "=A"(ret) );
    return ret;
}

double uptime()
{
    if (!timer_ready) return 0;

    static double initial_ticks = total_ticks();
    double ticks = total_ticks() - initial_ticks;

    return ticks / (double(clock_speed()) * 1'000'000.0); // MHz -> Hz
}

size_t epoch()
{
    static auto boot_date = Time::to_unix(rtc::get_time());
    static auto boot_ticks = Timer::ticks();

    return boot_date + (Timer::ticks() - boot_ticks)/Timer::freq();
}

Date get_time_of_day()
{
    return from_unix(epoch());
}

}
