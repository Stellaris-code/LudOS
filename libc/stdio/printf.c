/*
printf.c

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
#include <limits.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static bool print(const char* data, size_t length) {
        const unsigned char* bytes = (const unsigned char*) data;
        for (size_t i = 0; i < length; i++)
                if (putchar(bytes[i]) == EOF)
                        return false;
        return true;
}

int printf(const char* restrict format, ...)
{
        va_list parameters;
        va_start(parameters, format);

        int written = 0;

        while (*format != '\0') {
                size_t maxrem = INT_MAX - written;

                if (format[0] != '%' || format[1] == '%') {
                        if (format[0] == '%')
                                format++;
                        size_t amount = 1;
                        while (format[amount] && format[amount] != '%')
                                amount++;
                        if (maxrem < amount) {
                                // TODO: Set errno to EOVERFLOW.
                                return -1;
                        }
                        if (!print(format, amount))
                                return -1;
                        format += amount;
                        written += amount;
                        continue;
                }

                const char* format_begun_at = format++;

                if (*format == 'c') {
                        format++;
                        char c = (char) va_arg(parameters, int /* char promotes to int */);
                        if (!maxrem) {
                                // TODO: Set errno to EOVERFLOW.
                                return -1;
                        }
                        if (!print(&c, sizeof(c)))
                                return -1;
                        written++;
                } else if (*format == 's') {
                        format++;
                        const char* str = va_arg(parameters, const char*);
                        size_t len = strlen(str);
                        if (maxrem < len) {
                                // TODO: Set errno to EOVERFLOW.
                                return -1;
                        }
                        if (!print(str, len))
                                return -1;
                        written += len;
                } else {
                        format = format_begun_at;
                        size_t len = strlen(format);
                        if (maxrem < len) {
                                // TODO: Set errno to EOVERFLOW.
                                return -1;
                        }
                        if (!print(format, len))
                                return -1;
                        written += len;
                        format += len;
                }
        }

        va_end(parameters);
        return written;
}
