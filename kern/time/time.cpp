/*
time.cpp

Copyright (c) 21 Yann BOUCHER (yann)

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

#include <stdio.h>

namespace Time
{
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

bool leap_year(size_t year)
{
    if (year % 4 != 0) return false;
    else if (year % 100 != 0) return true;
    else if (year % 400 != 0) return false;
    else return true;
}

// FIXME
Date from_unix(size_t epoch)
{
    Date date;
    size_t dayclock = epoch % (60*60*24);
    size_t total_dayno = epoch / (60*60*24);
    size_t dayno = total_dayno;

    date.sec = dayclock % 60;
    date.min = (dayclock % 3600) / 60;
    date.hour = dayclock / 3600;

    date.year = 1970;

    while (dayno >= (leap_year(date.year) ? 366 : 365))
    {
        dayno -= (leap_year(date.year) ? 366 : 365);
        date.year++;
    }
    date.yday = dayno;

    for (size_t y { 1970 }; y <= date.year; ++y)
    {
        for (size_t m { 0 }; m < 12; ++m)
        {
            if (dayno < days_in_months[m] + (m == 1 && leap_year(y) ? 1 : 0))
            {
                date.month = m + 1;
                goto out;
            }
            dayno -= days_in_months[m] + (m == 1 && leap_year(y) ? 1 : 0);
        }
    }
    out:

    date.mday = dayno + 1;

    return date;
}

size_t to_unix(const Date &date)
{
    size_t epoch = 0;

    epoch += date.sec + (date.min * 60) + (date.hour * 3600);

    epoch += (date.yday-1) * secs_in_day;
    //epoch += (date.year - 1970) * secs_in_year;

    for (size_t year { date.year }; year > 1970; --year)
    {
        size_t days = 365;

        // Leap years
        if (year % 4 != 0);
        else if (year % 100 != 0) days = 366;
        else if (year % 400 != 0);
        else days = 366;

        epoch += days * secs_in_day;
    }

    return epoch;
}

const char* to_string(const Date &date)
{
    static char buf[256];

    ksnprintf(buf, sizeof(buf), "%d/%d/%d %02d:%02d:%02d", date.mday, date.month, date.year,
                                   date.hour, date.min, date.sec);

    return buf;
}

}
