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

#include "i686/cpu/cpuinfo.hpp"
#include "i686/pc/devices/rtc.hpp"

namespace Time
{

bool timer_ready = false;

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

Date to_local_time(Date utc_date)
{
    // heure d'été
    if (utc_date.month >= 4 && utc_date.month <= 7)
    {
        ++utc_date.hour;
    }

    // UTC+1
    ++utc_date.hour;

    return utc_date;
}

Date get_time_of_day()
{
    return to_local_time(rtc::get_time());
}

// FIXME
Date from_unix(size_t epoch)
{
    Date date;
    size_t dayclock = epoch % (60*60*24);
    int dayno = epoch / (60*60*24);

    date.sec = dayclock % 60;
    date.min = (dayclock % 3600) / 60;
    date.hour = dayclock / 3600;

    date.year = 1970;
    while (dayno >= 365)
    {
        dayno -= 365;
        date.year++;
    }
    date.day = dayno;

    return to_local_time(date);
}

}
