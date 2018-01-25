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

#include "utils/nop.hpp"

#include <functional.hpp>
#include <list.hpp>
#include <algorithm.hpp>

#include "panic.hpp"
#include <stdio.h>

class Timer
{

    friend class PIT;

private:
    struct Callback
    {
        Callback() = default;

        Callback(uint32_t istart, uint32_t iduration, std::function<void()> icallback, bool ioneshot)
            : start(istart), duration(iduration), callback(icallback), oneshot(ioneshot), to_be_deleted(false)
        {

        }

        uint32_t start;
        uint32_t duration;
        std::function<void()> callback;
        bool oneshot;
        bool to_be_deleted { false };
    };

public:
    using CallbackHandle = std::list<Callback>::iterator;

    static inline void init()
    {

    }

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

    static inline bool sleep_until(std::function<bool()> callback, uint32_t timeout = 0)
    {
        uint32_t interval = timeout/(1000/freq());
        m_ticks = 0;
        while (!callback() && (timeout == 0 || m_ticks < interval)) { nop(); }

        return callback();
    }

    // time in ms
    static inline CallbackHandle register_callback(uint32_t duration, std::function<void()> callback, bool oneshot = true)
    {
        if (freq())
        {
            m_callbacks.emplace_back(m_ticks, duration/(1000/freq()), callback, oneshot);
            return std::prev(m_callbacks.end());
        }
        else
        {
            return m_callbacks.end();
        }
    }

    static inline void remove_callback(const CallbackHandle& it)
    {
        m_callbacks.erase(it);
    }

    static inline uint32_t ticks()
    {
        return m_ticks;
    }

    static inline uint32_t freq()
    {
        return m_freq;
    }

    static inline std::function<void(uint32_t)> m_set_frequency_callback;

private:
    static inline void irq_callback()
    {
        ++m_ticks;
        // do not always check, it is costly
        if ((m_ticks % 8) == 0) handle_callbacks();
    }

    static inline void handle_callbacks()
    {
        for (auto& callback : m_callbacks)
        {
            if (m_ticks - callback.start >= callback.duration)
            {
                callback.callback();
                callback.start = m_ticks;
                if (callback.oneshot)
                {
                    callback.to_be_deleted = true;
                }
            }
        }

        // remove used callbacks
        m_callbacks.remove_if([](const Callback& c) { return c.to_be_deleted;});
    }
private:


    static inline std::list<Callback> m_callbacks;
    static inline uint32_t m_ticks { 0 };
    static inline uint32_t m_freq { 0 };
};

#endif // TIMER_HPP
