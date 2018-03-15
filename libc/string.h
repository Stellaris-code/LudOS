/*
string.h

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
#ifndef L_STRING_H
#define L_STRING_H 1

#include <sys/cdefs.h>

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

int memcmp(const void*, const void*, size_t);

#ifdef ARCH_i686
#include "arch/i686/memcpy.h"
#endif

void* memmove(void*, const void*, size_t);
void* _naive_memset(void*, uint8_t, size_t);
void _naive_double_memsetl(void* buf1ptr, void* buf2ptr, uint32_t value, size_t size);
extern void* (*memset)(void*, uint8_t, size_t);
extern void* (*aligned_memsetl)(void*, uint32_t , size_t );
extern void (*aligned_double_memsetl)(void*buf1ptr, void*buf2ptr, uint32_t value, size_t size);
void* _aligned_memsetl_sse2(void * dest, uint32_t val, size_t n);
void _aligned_double_memsetl_sse2(void*buf1ptr, void*buf2ptr, uint32_t val, size_t n);
void* memsetw(void*, uint16_t, size_t);
void* memsetl(void*bufptr, uint32_t value, size_t size);
void *memchr(const void *s, int c, size_t n);
size_t strlen(const char*);
char *strcpy(char * __restrict dest, const char * __restrict src);
char *strncpy(char * __restrict dest, const char * __restrict src, size_t n);
char *strcat ( char * destination, const char * source );
char *strncat ( char * destination, const char * source, size_t n);
int strcmp(const char* str1, const char* str2);
int strncmp(const char* str1, const char* str2, size_t n);
char * strdup(const char *str1);
const char* strstr( const char* str, const char* target );
char * strerror(int errnum);
char * _strerror_r(
        int errnum,
        int internal,
        int *errptr);
int posix_memalign(void **res, size_t align, size_t len)
#ifdef __cplusplus
throw()
#endif
;

long long strtoll(const char *str, char **endptr, int base);
unsigned long long strtoull(const char *str, char **endptr, int base);
long strtol(const char *str, char **endptr, int base);
unsigned long strtoul(const char *str, char **endptr, int base);

float strtof(const char* str, char** endptr);
double strtod(const char* str, char** endptr);
long double strtold(const char* str, char** endptr);

#ifdef __cplusplus
}
#endif

#endif
