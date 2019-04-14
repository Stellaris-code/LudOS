/*
stdio.h

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
#ifndef L_STDIO_H
#define L_STDIO_H 1

#include <sys/cdefs.h>
#include <stdbool.h>
#include <stdint.h>

#include "utils/defs.hpp"

#define EOF (-1)

#ifdef __cplusplus
void putcharw(char32_t c);
#endif

#ifdef __cplusplus
extern "C" {
#endif

extern bool putc_serial;

typedef struct
{
    size_t fd;
} FILE;

void putchar(char c);

void puts(const char*);

int fprintf(FILE * stream, const char * format, ...) PRINTF_FMT(2, 3);
FILE * fopen(const char * filename, const char * mode);
int fclose( FILE * stream );
int fflush( FILE * stream );

void perror(const char * str);

int getchar();

extern FILE* stdin;
extern FILE* stdout;
extern FILE* stderr;

#include "stdio/tinyprintf.h"

#ifdef __cplusplus
}
#endif

#endif
