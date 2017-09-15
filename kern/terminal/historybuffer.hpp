/*
historybuffer.hpp

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
#ifndef HISTORYBUFFER_HPP
#define HISTORYBUFFER_HPP

#include <stdint.h>

#include "utils/dynarray.hpp"
#include "utils/vector.hpp"

class HistoryBuffer
{
public:
    HistoryBuffer(size_t line_width, size_t height);

    uint16_t get_char(size_t x, size_t y) const;
    void add(const vector<uint16_t> &line);

    size_t size() const
    {
        if (full())
        {
            return m_data.size()/m_line_width;
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

private:
    const size_t m_line_width;
    const size_t m_height;

    size_t m_front { 0 };
    bool m_full { false };

    vector<uint16_t> m_data;
};

#endif // HISTORYBUFFER_HPP
