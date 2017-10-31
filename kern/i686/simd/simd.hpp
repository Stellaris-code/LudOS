/*
sse.hpp

Copyright (c) 29 Yann BOUCHER (yann)

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
#ifndef SSE_HPP
#define SSE_HPP

#include <stdint.h>

enum SIMDType : uint16_t
{
    MMX  =1<<0,
    SSE  =1<<1,
    SSE2 =1<<2,
    SSE3 =1<<3,
    SSSE3=1<<4,
    SSE41=1<<5,
    SSE42=1<<6,
    SSE4A=1<<7,
    XOP  =1<<8,
    FMA4 =1<<9,
    CVT16=1<<10,
    AVX  =1<<11,
    XSAVE=1<<12,
    AVX512=1<<13
};

uint16_t simd_features();

extern "C"
void enable_sse();

#endif // SSE_HPP
