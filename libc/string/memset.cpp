/*
memset.c

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

#include "utils/logging.hpp"
#include "utils/align.hpp"

void* _naive_memset(void* bufptr, uint8_t value, size_t size)
{
    uint8_t* buf = reinterpret_cast<uint8_t*>(bufptr);
    for (size_t i = 0; i < size; i++)
    {
        buf[i] = value;
    }
    return bufptr;
}

void _naive_double_memsetl(void* buf1ptr, void* buf2ptr, uint32_t value, size_t size)
{
    uint32_t* buf1 = reinterpret_cast<uint32_t*>(buf1ptr);
    uint32_t* buf2 = reinterpret_cast<uint32_t*>(buf2ptr);
    for (size_t i = 0; i < size/sizeof(uint32_t); i++)
    {
        buf1[i] = buf2[i] = value;
    }
}

void* memsetw(void* bufptr, uint16_t value, size_t size)
{
    uint16_t* buf = reinterpret_cast<uint16_t*>(bufptr);
    for (size_t i = 0; i < size/sizeof(uint16_t); i++)
    {
        buf[i] = value;
    }
    return bufptr;
}

void* memsetl(void* bufptr, uint32_t value, size_t size)
{
    uint32_t* buf = reinterpret_cast<uint32_t*>(bufptr);
    for (size_t i = 0; i < size/sizeof(uint32_t); i++)
    {
        buf[i] = value;
    }
    return bufptr;
}

void * _repmovsb_memset(void * dest, uint8_t val, size_t n) {
    int d0, d1;
    asm volatile (
                "rep; stosb;"
                : "=&c" (d0), "=&D" (d1)
                : "0" (n), "a" (val), "1" (dest)
                : "memory");
    return dest;
}

void* _aligned_memsetl_sse2(void * dest, uint32_t val, size_t n)
{
    alignas(32) uint32_t xmm0[4];
    memsetl(xmm0, val, 4*sizeof(uint32_t));

    uint8_t* to = (uint8_t*)dest;

    __asm__ __volatile__ ("movdqa (%0), %%xmm0\n"::"r"(xmm0):"memory");

    for(size_t i = 0; i < n / 128; i++)
    {
        __asm__ __volatile__ (
                    "\tmovntdq %%xmm0, (%0)\n"
                    "\tmovntdq %%xmm0, 16(%0)\n"
                    "\tmovntdq %%xmm0, 32(%0)\n"
                    "\tmovntdq %%xmm0, 48(%0)\n"
                    "\tmovntdq %%xmm0, 64(%0)\n"
                    "\tmovntdq %%xmm0, 80(%0)\n"
                    "\tmovntdq %%xmm0, 96(%0)\n"
                    "\tmovntdq %%xmm0, 112(%0)\n"
                    : : "r" (to) : "memory");
        to += 128;
    }

    return dest;
}

void _aligned_double_memsetl_sse2(void*buf1ptr, void*buf2ptr, uint32_t val, size_t n)
{
    alignas(32) uint32_t xmm0[4];
    memsetl(xmm0, val, 4*sizeof(uint32_t));

    uint8_t* to1 = (uint8_t*)buf1ptr;
    uint8_t* to2 = (uint8_t*)buf2ptr;

    __asm__ __volatile__ ("movdqa (%0), %%xmm0\n"::"r"(xmm0):"memory");

    for(int i = 0; i < n / 128; i++)
    {
        __asm__ __volatile__ (
                    "\tmovntdq %%xmm0, (%0)\n"
                    "\tmovntdq %%xmm0, 16(%0)\n"
                    "\tmovntdq %%xmm0, 32(%0)\n"
                    "\tmovntdq %%xmm0, 48(%0)\n"
                    "\tmovntdq %%xmm0, 64(%0)\n"
                    "\tmovntdq %%xmm0, 80(%0)\n"
                    "\tmovntdq %%xmm0, 96(%0)\n"
                    "\tmovntdq %%xmm0, 112(%0)\n"
                    "\tmovntdq %%xmm0, (%1)\n"
                    "\tmovntdq %%xmm0, 16(%1)\n"
                    "\tmovntdq %%xmm0, 32(%1)\n"
                    "\tmovntdq %%xmm0, 48(%1)\n"
                    "\tmovntdq %%xmm0, 64(%1)\n"
                    "\tmovntdq %%xmm0, 80(%1)\n"
                    "\tmovntdq %%xmm0, 96(%1)\n"
                    "\tmovntdq %%xmm0, 112(%1)\n"
                    : : "r" (to1), "r" (to2) : "memory");
        to1 += 128;
        to2 += 128;
    }
}

void* (*memset)(void*, uint8_t, size_t) = _naive_memset;
void* (*aligned_memsetl)(void*bufptr, uint32_t value, size_t size) = memsetl;
void (*aligned_double_memsetl)(void*buf1ptr, void*buf2ptr, uint32_t value, size_t size) = _naive_double_memsetl;
