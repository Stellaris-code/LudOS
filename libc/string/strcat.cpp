/*
strcat.cpp

Copyright (c) 13 Yann BOUCHER (yann)

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

char *strcat ( char * destination, const char * source )
{
    const size_t dest_len = strlen(destination);
    const size_t src_len = strlen(source);

    for (size_t i { 0 }; i < src_len; ++i)
    {
        destination[dest_len + i] = source[i];
    }

    destination[dest_len+src_len] = '\0';

    return destination;
}

char *strncat(char *s1, const char *s2, size_t n)
{
    char *os1;

    os1 = s1;
    while (*s1++)
        ;
    --s1;
    while ((*s1++ = *s2++))
        if (--n < 0)
        {
            *--s1 = '\0';
            break;
        }
    return (os1);
}
