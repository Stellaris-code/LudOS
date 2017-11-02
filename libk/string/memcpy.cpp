/*
memcpy.c

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
#include <string.h>
#include "halt.hpp"

void* (*memcpy)(void* __restrict, const void* __restrict, size_t) = _naive_memcpy;

void* _naive_memcpy(void* __restrict dstptr, const void* __restrict srcptr, size_t size)
{
    unsigned char* dst = reinterpret_cast<unsigned char*>(dstptr);
    const unsigned char* src = reinterpret_cast<const unsigned char*>(srcptr);
    for (size_t i = 0; i < size; i++)
        dst[i] = src[i];
    return dstptr;
}

/* From Linux 2.4.8.  I think this must be aligned. */
void *
_memcpy_mmx (void *v_to, const void *v_from, size_t len)
{
    int i;
    uint8_t* to = (uint8_t*)v_to;
    uint8_t* from = (uint8_t*)v_from;

    for(i = 0; i < len / 64; i++) {
            __asm__ __volatile__ (
           "movq (%0), %%mm0\n"
           "\tmovq 8(%0), %%mm1\n"
           "\tmovq 16(%0), %%mm2\n"
           "\tmovq 24(%0), %%mm3\n"
           "\tmovq %%mm0, (%1)\n"
           "\tmovq %%mm1, 8(%1)\n"
           "\tmovq %%mm2, 16(%1)\n"
           "\tmovq %%mm3, 24(%1)\n"
           "\tmovq 32(%0), %%mm0\n"
           "\tmovq 40(%0), %%mm1\n"
           "\tmovq 48(%0), %%mm2\n"
           "\tmovq 56(%0), %%mm3\n"
           "\tmovq %%mm0, 32(%1)\n"
           "\tmovq %%mm1, 40(%1)\n"
           "\tmovq %%mm2, 48(%1)\n"
           "\tmovq %%mm3, 56(%1)\n"
           : : "r" (from), "r" (to) : "memory");
        from += 64;
        to += 64;
    }

    if (len & 63)
        _naive_memcpy(to, from, len & 63);

    return to;
}
