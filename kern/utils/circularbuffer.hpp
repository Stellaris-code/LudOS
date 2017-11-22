/*
circularbuffer.hpp

Copyright (c) 15 Yann BOUCHER (yann)

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
#ifndef CIRCULARBUFFER_HPP
#define CIRCULARBUFFER_HPP

#include <stdint.h>
#include <vector.hpp>
#include <assert.h>

template <typename T>
class CircularBuffer
{
public:
    CircularBuffer(size_t height)
        : m_data(height)
    {}

    const T& get_entry(size_t idx) const
    {
        if (full())
        {
            assert(idx < m_data.size());

            return m_data[(m_front + idx) % m_data.size()];
        }
        else
        {
            assert(idx < m_data.size());
            return m_data[idx];
        }
    }
    T& get_entry(size_t idx)
    {
        if (full())
        {
            assert(idx < m_data.size());

            return m_data[(m_front + idx) % m_data.size()];
        }
        else
        {
            assert(idx < m_data.size());
            return m_data[idx];
        }
    }

    void add(T entry)
    {
        m_data[m_front++] = entry;

        if (m_front == m_data.size())
        {
            m_front = 0;
            m_full = true;
        }
    }

    size_t size() const
    {
        if (full())
        {
            return m_data.size();
        }
        else
        {
            return m_front;
        }
    }

    bool full() const
    {
        return m_full;
    }

    void clear()
    {
        m_full = false;
        m_front = 0;
    }

private:
    size_t m_front { 0 };
    bool m_full { false };

    std::vector<T> m_data;
};

#endif // CIRCULARBUFFER_HPP
