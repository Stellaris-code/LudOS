/*
bitops.hpp

Copyright (c) 28 Yann BOUCHER (yann)

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
#ifndef BITOPS_HPP
#define BITOPS_HPP

#include "utils/stdint.h"

template <typename T>
inline void bit_set(T& val, size_t pos)
{
    val |= 1 << pos;
}

template <typename T>
inline void bit_clear(T& val, size_t pos)
{
    val &= ~(1 << pos);
}

template <typename T>
inline void bit_toggle(T& val, size_t pos)
{
    val ^= 1 << pos;
}

template <typename T>
inline bool bit_check(T& val, size_t pos)
{
    return (val >> pos) & 1;
}

template <typename T>
inline void bit_change(T& val, bool bit, size_t pos)
{
    val ^= (-bit ^ val) & (1 << pos);
}

#endif // BITOPS_HPP
