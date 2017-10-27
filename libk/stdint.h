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
#ifndef STDINT_HPP
#define STDINT_HPP

#ifdef CODE_MODEL_PASS // Aliases for Qt Creator IDE
#error Only for clang code model pass !
typedef unsigned char uint8_t;
typedef char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned long uint64_t;
typedef long int64_t;
typedef uint64_t size_t;
typedef int32_t intptr_t;
typedef uint64_t uintptr_t;
#endif

#include_next <stdint.h>
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
#endif

#endif // STDINT_HPP
