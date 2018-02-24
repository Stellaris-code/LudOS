/*
time.hpp

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
#ifndef TIME_HPP
#define TIME_HPP

#include <stdint.h>

namespace Time
{

struct Date
{
    uint32_t sec {};
    uint32_t min {};
    uint32_t hour {};
    uint32_t mday {};
    uint32_t yday {};
    uint32_t month {};
    uint32_t year {};
};

static constexpr size_t secs_in_day = 86400;
static constexpr size_t secs_in_week = 604800;
static constexpr size_t secs_in_month = 2629743;
static constexpr size_t secs_in_year = 31556926;

static constexpr uint32_t days_in_months[12] =
{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };

extern bool timer_ready;

Date get_time_of_day();
Date to_local_time(Date utc_date);

Date from_unix(size_t epoch);
size_t to_unix(const Date& date);

double uptime();
uint64_t total_ticks();

size_t epoch();

const char *to_string(const Date& date);

}

#endif // TIME_HPP
