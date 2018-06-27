/*
utf8decoder.cpp

Copyright (c) 07 Yann BOUCHER (yann)

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

#include "utf8decoder.hpp"

#include <array.hpp>
#include <kstring/kstring.hpp>

#include "utils/logging.hpp"

void UTF8Decoder::feed(uint8_t byte)
{
    if (m_completed) return;

    if (m_current_length == 0)
    {
        m_length = get_length(byte);

        m_current = byte;

        if (m_length == 1)
        {
            m_completed = true;
            return;
        }

        switch (m_length)
        {
            case 4:
                m_current ^= 0xf0;
                break;
            case 3:
                m_current ^= 0xe0;
                break;
            case 2:
                m_current ^= 0xc0;
                break;
        }
    }
    else
    {
        m_current <<= 6;
        m_current |= (byte ^ 0x80);
    }

    ++m_current_length;

    if (m_current_length == m_length)
    {
        m_completed = true;
    }
}

void UTF8Decoder::reset()
{
    m_completed = false;
    m_length = 0;
    m_current_length = 0;
    m_current = invalid;
}

char32_t UTF8Decoder::spit()
{
    if (!m_completed)
    {
        return invalid;
    }

    m_completed = false;
    auto value = m_current;
    reset();
    return value;
}

void UTF8Decoder::invalidate()
{
    reset();
    m_current = invalid;
}

size_t UTF8Decoder::get_length(uint8_t first_byte)
{
    size_t l;
    uint8_t c = first_byte;
    c >>= 3;
    // 6 => 0x7e
    // 5 => 0x3e
    if (c == 0x1e)
    {
        l = 4;
    }
    else
    {
        c >>= 1;
        if (c == 0xe)
        {
            l = 3;
        }
        else
        {
            c >>= 1;
            if (c == 0x6)
            {
                l = 2;
            }
            else {
                l = 1;
            }
        }
    }
    return l;
}

kpp::u32string u8_decode(const kpp::string &str)
{
    kpp::u32string kresult;
    UTF8Decoder decoder;

    for (auto c : str)
    {
        decoder.feed(c);
        if (decoder.ready())
        {
            kresult += decoder.spit();
        }
    }

    return kresult;
}
