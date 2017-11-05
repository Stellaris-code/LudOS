/*
sse.cpp

Copyright (c) 30 Yann BOUCHER (yann)

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

#include "simd.hpp"

#include "i686/cpu/cpuid.hpp"

uint16_t simd_features()
{
    uint32_t ecx, edx, unused;
    cpuid(1, unused, unused, ecx, edx);

    uint16_t result = 0;

    if (edx & (1<<25)) result |= SSE;
    if (edx & (1<<26)) result |= SSE2;
    if (ecx & (1<< 0)) result |= SSE3;
    if (ecx & (1<< 9)) result |= SSSE3;
    if (ecx & (1<<19)) result |= SSE41;
    if (ecx & (1<<20)) result |= SSE42;
    if (ecx & (1<< 6)) result |= SSE4A;

    if (ecx & (1<<11)) result |= XOP;
    if (ecx & (1<<16)) result |= FMA4;
    if (ecx & (1<<29)) result |= CVT16;
    if (ecx & (1<<28)) result |= AVX;
    if (ecx & (1<<26)) result |= XSAVE;

    return result;
}
