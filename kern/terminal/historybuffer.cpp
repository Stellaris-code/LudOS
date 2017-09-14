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
    : m_line_width(line_width), m_height(height),
      m_data(line_width*height)
{

}

uint16_t HistoryBuffer::get_char(size_t x, size_t y) const
{
    const size_t index = y*m_line_width + x;

    if (full())
    {
        assert_msg(index < m_data.size(), "Invalid access of history buffer %p at index %zd !", this, index);

        return m_data[(m_front + index) % m_data.size()];
    }
    else
    {
        assert_msg(index < m_data.size(), "Invalid access of history buffer %p at index %zd with m_front %zd !", this, index, m_front);
        return m_data[index];
    }
}

void HistoryBuffer::add(uint16_t *line)
{
    memcpy(&m_data[m_front*m_line_width], line, m_line_width*sizeof(uint16_t));

    ++m_front;

    if (m_front == m_height)
    {
        m_front = 0;
        m_full = true;
    }
}
