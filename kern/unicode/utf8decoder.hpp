/*
utf8decoder.hpp

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
#ifndef UTF8DECODER_HPP
#define UTF8DECODER_HPP

#include <stdint.h>

#include <kstring/kstrfwd.hpp>

class UTF8Decoder
{
public:
    void feed(uint8_t byte);
    bool ready() const { return m_completed; }

    void reset();

    char32_t spit();

private:
    void invalidate();
    size_t get_length(uint8_t first_byte);

public:
    const char32_t invalid { 0xfffd };

private:
    bool m_completed { false };
    size_t m_current_length { 0 };
    size_t m_length { 0 };
    char32_t m_current { 0 };
};

kpp::u32string u8_decode(kpp::string_view str);

#endif // UTF8DECODER_HPP
