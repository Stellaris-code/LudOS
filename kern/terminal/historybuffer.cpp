/*
historybuffer.cpp

Copyright (c) 11 Yann BOUCHER (yann)

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

#include "historybuffer.hpp"

#include <assert.h>
#include <string.h>

HistoryBuffer::HistoryBuffer(size_t line_width, size_t height)
    : m_line_width(line_width), m_data(height)
{
}

uint16_t HistoryBuffer::get_char(size_t x, size_t y) const
{
    if (full())
    {
        assert_msg(y < m_data.size() && x < m_data[0].size(), "Invalid access of history buffer %p at (%zd, %zd) !", this, x, y);

        return m_data[(m_front + y) % m_data.size()][x];
    }
    else
    {
        assert_msg(y < m_data.size() && x < m_data[0].size(), "Invalid access of history buffer %p at (%zd, %zd) with m_front %zd !", this, x, y, m_front);
        return m_data[y][x];
    }
}

void HistoryBuffer::add(const std::vector<uint16_t>& line)
{
    m_data[m_front] = line;

    ++m_front;

    if (m_front == m_data.size())
    {
        m_front = 0;
        m_full = true;
    }
}
