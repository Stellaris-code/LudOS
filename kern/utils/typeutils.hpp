/*
typeutils.hpp

Copyright (c) 04 Yann BOUCHER (yann)

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
#ifndef TYPEUTILS_HPP
#define TYPEUTILS_HPP

#include <stdint.h>

template <size_t size>
struct uintn
{
    static_assert("Invalid uintn_t size !");
    using type = void;
};

template <>
struct uintn<8>
{
    using type = uint8_t;
};

template <>
struct uintn<16>
{
    using type = uint16_t;
};

template <>
struct uintn<32>
{
    using type = uint32_t;
};

template <>
struct uintn<64>
{
    using type = uint64_t;
};

template <size_t size>
using uintn_t = typename uintn<size>::type;

#endif // TYPEUTILS_HPP
