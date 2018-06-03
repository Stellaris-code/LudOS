/*
utf32decoder.cpp

Copyright (c) 23 Yann BOUCHER (yann)

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

#include "utf32decoder.hpp"

#include <kstring/kstring.hpp>

kpp::string decode_utf32(char32_t ch)
{
    kpp::string utf8;

    size_t len { 0 };

    if (ch < 0x80)
    {
        len = 1;
        utf8 += ch;
    }
    else if (ch < 0x800)
    {
        len = 2;
        utf8 += 0xc0 | (ch >> 6);
    }
    else if (ch < 0x10000)
    {
        len = 3;
        utf8 += 0xe0 | (ch >> 12);
    }
    else if (ch < 0x200000)
    {
        len = 4;
        utf8 += 0xf0 | (ch >> 18);
    }

    if (len > 1)
    {
        for (size_t i = len; i > 1; i--)
        {
            utf8 += 0x80 | (ch & (0x3f << ((i-2)*6)));
        }
    }

    return utf8;
}
