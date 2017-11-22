/*
memcpy.h

Copyright (c) 15 Yann BOUCHER (yann)

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
#ifndef MEMCPY_H
#define MEMCPY_H

#include <stdint.h>

void* _naive_memcpy(void* __restrict dstptr, const void* __restrict srcptr, size_t size);
void * _memcpy_mmx (void *v_to, const void *v_from, size_t len);
void * _memcpy_sse2 (void *v_to, const void *v_from, size_t len);
void * _aligned_memcpy_sse2 (void * __restrict v_to, const void * __restrict v_from, size_t len);

extern void* (*memcpy)(void* __restrict, const void* __restrict, size_t);
extern void* (*memcpyl)(void* __restrict, const void* __restrict, size_t);
extern void* (*aligned_memcpy)(void* __restrict, const void* __restrict, size_t);

inline void *constant_memcpy(void *to, const void *from, size_t n)
{
    int d0, d1, d2;
    __asm__ __volatile__("rep ; movsl\n\t"
             "movl %4,%%ecx\n\t"
             "andl $3,%%ecx\n\t"
             "jz 1f\n\t"
             "rep ; movsb\n\t"
             "1:"
             : "=&c" (d0), "=&D" (d1), "=&S" (d2)
             : "0" (n / 4), "g" (n), "1" ((long)to), "2" ((long)from)
             : "memory");
return to;
}

#endif // MEMCPY_H
