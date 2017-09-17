/*
wops.c

Copyright (c) 17 Yann BOUCHER (yann)

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

#include <wchar.h>

wchar_t *wmemcpy(wchar_t *d, const wchar_t *s, size_t n)
{
        wchar_t *a = d;
        while (n--) *d++ = *s++;
        return a;
}

wchar_t *wmemmove(wchar_t *d, const wchar_t *s, size_t n)
{
        if ((size_t)(d-s) < n) {
                while (n--) d[n] = s[n];
                return d;
        }
        return wmemcpy(d, s, n);
}


wchar_t *wmemset(wchar_t *d, wchar_t c, size_t n)
{
        wchar_t *ret = d;
        while (n--) *d++ = c;
        return ret;
}

int wmemcmp(const wchar_t *l, const wchar_t *r, size_t n)
{
        for (; n && *l==*r; n--, l++, r++);
        return n ? *l-*r : 0;
}

wchar_t *wmemchr(const wchar_t *s, wchar_t c, size_t n)
{
        for (; n && *s != c; n--, s++);
        return n ? (wchar_t *)s : 0;
}
