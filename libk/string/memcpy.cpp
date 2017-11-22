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
#include "utils/align.hpp"

#define free kfree
#define malloc kmalloc
#include "emmintrin.h"

void* _naive_memcpy(void* __restrict dstptr, const void* __restrict srcptr, size_t size)
{
    unsigned char* dst = reinterpret_cast<unsigned char*>(dstptr);
    const unsigned char* src = reinterpret_cast<const unsigned char*>(srcptr);
    for (size_t i = 0; i < size; i++)
        dst[i] = src[i];
    return dstptr;
}

void * _repmovsb_memcpy(void * __restrict dest, const void * __restrict src, size_t n) {
    asm volatile("rep movsb"
                 : "+S"(src), "+D"(dest)
                 : "c"(n)
                 : "flags", "memory");
    return dest;
}

void * _repmovsl_memcpy(void * __restrict dest, const void * __restrict src, size_t n) {
    asm volatile("rep movsl"
                 : "+S"(src), "+D"(dest)
                 : "c"(n/4)
                 : "cc", "memory");
    return dest;
}

/* From Linux 2.4.8.  I think this must be aligned. */
void *
_memcpy_mmx (void * __restrict v_to, const void * __restrict v_from, size_t len)
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

    __asm__ __volatile__ ("emms"); // Reset fpu registers for normal use

    if (len & 63)
        _naive_memcpy(to, from, len & 63);

    return to;
}

void *
_aligned_memcpy_sse2 (void * __restrict v_to, const void * __restrict v_from, size_t len)
{
    int i;
    uint8_t* to = (uint8_t*)v_to;
    uint8_t* from = (uint8_t*)v_from;

    for(i = 0; i < len / 128; i++)
    {
        __builtin_prefetch(from, 0);

        __asm__ __volatile__ (
                    "movdqa (%0), %%xmm0\n"
                    "\tmovdqa 16(%0), %%xmm1\n"
                    "\tmovdqa 32(%0), %%xmm2\n"
                    "\tmovdqa 48(%0), %%xmm3\n"
                    "\tmovdqa 64(%0), %%xmm4\n"
                    "\tmovdqa 80(%0), %%xmm5\n"
                    "\tmovdqa 96(%0), %%xmm6\n"
                    "\tmovdqa 112(%0), %%xmm7\n"
                    "\tmovntdq %%xmm0, (%1)\n"
                    "\tmovntdq %%xmm1, 16(%1)\n"
                    "\tmovntdq %%xmm2, 32(%1)\n"
                    "\tmovntdq %%xmm3, 48(%1)\n"
                    "\tmovntdq %%xmm4, 64(%1)\n"
                    "\tmovntdq %%xmm5, 80(%1)\n"
                    "\tmovntdq %%xmm6, 96(%1)\n"
                    "\tmovntdq %%xmm7, 112(%1)\n"
                    : : "r" (from), "r" (to) : "memory");
        from += 128;
        to += 128;
    }

    return to;
}

// We don't have SSE at the very beggining, use rep movsb version
void* (*memcpy)(void* __restrict, const void* __restrict, size_t) = _repmovsb_memcpy;
void* (*memcpyl)(void* __restrict, const void* __restrict, size_t) = _repmovsl_memcpy;
void* (*aligned_memcpy)(void* __restrict, const void* __restrict, size_t) = _naive_memcpy;
