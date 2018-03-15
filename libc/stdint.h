/*
qtcreatorstdint.hpp

Copyright (c) 22 Yann BOUCHER (yann)

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
#ifndef L_STDINT_HPP
#define L_STDINT_HPP

typedef unsigned char uint8_t;
typedef char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned int uint32_t;
typedef uint32_t uint_fast32_t;
typedef int int32_t;
typedef unsigned long long uint64_t;
typedef long long int64_t;
typedef int intptr_t;
typedef unsigned int uintptr_t;

#include <stddef.h>

#ifdef __cplusplus
static_assert(sizeof(uint8_t) == 1);
static_assert(sizeof(int8_t) == 1);
static_assert(sizeof(uint16_t) == 2);
static_assert(sizeof(int16_t) == 2);
static_assert(sizeof(uint32_t) == 4);
static_assert(sizeof(int32_t) == 4);
static_assert(sizeof(uint64_t) == 8);
static_assert(sizeof(int64_t) == 8);

namespace std
{
typedef decltype(nullptr) nullptr_t;
}
#endif

#endif // STDINT_HPP
