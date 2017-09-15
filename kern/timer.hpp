/*
timer.hpp

Copyright (c) 26 Yann BOUCHER (yann)

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
#ifndef TIMER_HPP
#define TIMER_HPP

#include <stdint.h>
#include "i686/pc/registers.hpp"

#include "nop.hpp"

#include "panic.hpp"
#include <stdio.h>

class Timer
{
public:
    static inline void set_frequency(uint32_t freq)
    {
        Timer::m_freq = freq;
        if (!m_set_frequency_callback)
        {
            panic("set_frequency_callback is not set !");
        }
        m_set_frequency_callback(freq);
    }

    // time in ms
    static inline void sleep(uint32_t time)
    {
        uint32_t interval = time/(1000/freq());
        m_ticks = 0;
        while (m_ticks < interval) { nop(); }
    }

    static inline uint32_t ticks()
    {
        return m_ticks;
    }

    static inline uint32_t freq()
    {
        return m_freq;
    }

    static inline void(*m_set_frequency_callback)(uint32_t); // set_frequency
    static inline uint32_t m_ticks { 0 };
    static inline uint32_t m_freq { 0 };
};

#endif // TIMER_HPP
