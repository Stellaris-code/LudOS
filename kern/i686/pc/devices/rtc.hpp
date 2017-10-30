/*
rtc.hpp

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
#ifndef RTC_HPP
#define RTC_HPP

#include <stdint.h>

#include "time/time.hpp"
#include "utils/nop.hpp"
#include "utils/defs.hpp"
#include "utils/logging.hpp"
#include "io.hpp"

#include "acpi.h"

namespace rtc
{

static constexpr uint16_t cmos_addr = 0x70;
static constexpr uint16_t cmos_data = 0x71;

namespace detail
{

inline uint8_t century_register()
{
    return AcpiGbl_FADT.Century;
}

inline uint8_t get_register(uint8_t reg)
{
    outb(cmos_addr, reg);
    return inb(cmos_data);
}

inline void wait_update()
{
    while (true)
    {
        outb(cmos_addr, 0x0A);
        if (!(inb(cmos_data) & 0x80))
        {
            return;
        }
    }
}
}

inline Date get_time()
{
    uint8_t century = 0;
    uint8_t last_second;
    uint8_t last_minute;
    uint8_t last_hour;
    uint8_t last_day;
    uint8_t last_month;
    uint8_t last_year;
    uint8_t last_century;
    uint8_t registerB;
    // Note: This uses the "read registers until you get the same values twice in a row" technique
    //       to avoid getting dodgy/inconsistent values due to RTC updates

    detail::wait_update();
    uint32_t second = detail::get_register(0x00);
    uint32_t minute = detail::get_register(0x02);
    uint32_t hour = detail::get_register(0x04);
    uint32_t day = detail::get_register(0x07);
    uint32_t month = detail::get_register(0x08);
    uint32_t year = detail::get_register(0x09);
    if(detail::century_register())
    {
        century = detail::get_register(detail::century_register());
    }

    do {
        last_second = second;
        last_minute = minute;
        last_hour = hour;
        last_day = day;
        last_month = month;
        last_year = year;
        last_century = century;

        detail::wait_update();
        second = detail::get_register(0x00);
        minute = detail::get_register(0x02);
        hour = detail::get_register(0x04);
        day = detail::get_register(0x07);
        month = detail::get_register(0x08);
        year = detail::get_register(0x09);
        if(detail::century_register())
        {
            century = detail::get_register(detail::century_register());
        }
    } while( (last_second != second) || (last_minute != minute) || (last_hour != hour) ||
             (last_day != day) || (last_month != month) || (last_year != year) ||
             (last_century != century) );

    registerB = detail::get_register(0x0B);

    // Convert BCD to binary values if necessary

    if (!(registerB & 0x04))
    {
        second = (second & 0x0F) + ((second / 16) * 10);
        minute = (minute & 0x0F) + ((minute / 16) * 10);
        hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
        day = (day & 0x0F) + ((day / 16) * 10);
        month = (month & 0x0F) + ((month / 16) * 10);
        year = (year & 0x0F) + ((year / 16) * 10);
        if(detail::century_register())
        {
            century = (century & 0x0F) + ((century / 16) * 10);
        }
    }

    // Convert 12 hour clock to 24 hour clock if necessary

    if (!(registerB & 0x02) && (hour & 0x80))
    {
        hour = ((hour & 0x7F) + 12) % 24;
    }

    // Calculate the full (4-digit) year

    if(detail::century_register())
    {
        year += century * 100;
    }
    else
    {
        year += (CURRENT_YEAR / 100) * 100;
        if(year < CURRENT_YEAR) year += 100;
    }

    return Date{second, minute, hour, day, month, year};
}

inline void init()
{
    Time::get_time_of_day_callback = []{ return get_time(); };
}

}

#endif // RTC_HPP
