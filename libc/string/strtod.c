/*
strtod.c

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

#include <string.h>
#include <ctype.h>
#include <wchar.h>

long double strtold(const char* str, char** endptr){
    long double result = 0.0;
    char signedResult = '\0';
    char signedExponent = '\0';
    int decimals = 0;
    bool isExponent = false;
    bool hasExponent = false;
    bool hasResult = false;
    // exponent is logically int but is coded as long double so that its eventual
    // overflow detection can be the same as for long double result
    long double exponent = 0;
    char c;

    for (; '\0' != (c = *str); ++str) {
        if ((c >= '0') && (c <= '9')) {
            int digit = c - '0';
            if (isExponent) {
                exponent = (10 * exponent) + digit;
                hasExponent = true;
            } else if (decimals == 0) {
                result = (10 * result) + digit;
                hasResult = true;
            } else {
                result += (long double)digit / decimals;
                decimals *= 10;
            }
            continue;
        }

        if (c == '.') {
            if (!hasResult) break; // don't allow leading '.'
            if (isExponent) break; // don't allow decimal places in exponent
            if (decimals != 0) break; // this is the 2nd time we've found a '.'

            decimals = 10;
            continue;
        }

        if ((c == '-') || (c == '+')) {
            if (isExponent) {
                if (signedExponent || (exponent != 0)) break;
                else signedExponent = c;
            } else {
                if (signedResult || (result != 0)) break;
                else signedResult = c;
            }
            continue;
        }

        if (c == 'E') {
            if (!hasResult) break; // don't allow leading 'E'
            if (isExponent) break;
            else isExponent = true;
            continue;
        }

        break; // unexpected character
    }

    if (isExponent && !hasExponent) {
        while (*str != 'E')
            --str;
    }

    if (!hasResult && signedResult) --str;

    if (endptr) *endptr = (char*)(str);

    for (; exponent != 0; --exponent) {
        if (signedExponent == '-') result /= 10;
        else result *= 10;
    }

    if (signedResult == '-' && result != 0) result = -result;

    return result;
}

long double wcstold(const wchar_t* str, wchar_t** endptr){
    long double result = 0.0;
    wchar_t signedResult = '\0';
    wchar_t signedExponent = '\0';
    int decimals = 0;
    bool isExponent = false;
    bool hasExponent = false;
    bool hasResult = false;
    // exponent is logically int but is coded as long double so that its eventual
    // overflow detection can be the same as for long double result
    long double exponent = 0;
    wchar_t c;

    for (; '\0' != (c = *str); ++str) {
        if ((c >= '0') && (c <= '9')) {
            int digit = c - '0';
            if (isExponent) {
                exponent = (10 * exponent) + digit;
                hasExponent = true;
            } else if (decimals == 0) {
                result = (10 * result) + digit;
                hasResult = true;
            } else {
                result += (long double)digit / decimals;
                decimals *= 10;
            }
            continue;
        }

        if (c == '.') {
            if (!hasResult) break; // don't allow leading '.'
            if (isExponent) break; // don't allow decimal places in exponent
            if (decimals != 0) break; // this is the 2nd time we've found a '.'

            decimals = 10;
            continue;
        }

        if ((c == '-') || (c == '+')) {
            if (isExponent) {
                if (signedExponent || (exponent != 0)) break;
                else signedExponent = c;
            } else {
                if (signedResult || (result != 0)) break;
                else signedResult = c;
            }
            continue;
        }

        if (c == 'E') {
            if (!hasResult) break; // don't allow leading 'E'
            if (isExponent) break;
            else isExponent = true;
            continue;
        }

        break; // unexpected wchar_tacter
    }

    if (isExponent && !hasExponent) {
        while (*str != 'E')
            --str;
    }

    if (!hasResult && signedResult) --str;

    if (endptr) *endptr = (wchar_t*)(str);

    for (; exponent != 0; --exponent) {
        if (signedExponent == '-') result /= 10;
        else result *= 10;
    }

    if (signedResult == '-' && result != 0) result = -result;

    return result;
}

long long strtoll(const char *str, char **endptr, int base)
{
    int c, neg = 0;
    const char *alphabet = "0123456789abcdefghijklmnopqrstuvwxyz";
    long long val = 0;

    if (base != 0 && (base < 2 || base > 36)) return 0;

    /* skip whitespace */
    while (*str && isspace(*str))
        str++;

    /* parse potential -/+ (default to positive) */
    if (*str == '-')
    {
        neg = 1;
        str++;
    }
    else if (*str == '+')
    {
        str++;
    }

    /* parse prefix (0x, 0) */
    if (base == 0 || base == 16)
    {
        if (str[0] == '0' && str[1] == 'x')
        {
            base = 16;
            str += 2;
        }
    }
    if (base == 0)
    {
        if (str[0] == '0')
            base = 8;
    }
    if (base == 0)
        base = 10;

    /* parse number */
    while ((c = *str))
    {
        /* search for c */
        const char *idx = (const char*)memchr(alphabet, tolower(c), base);
        if (idx == 0)
            break;
        /* XXX clamp to LONG_MAX / LONG_MIN */
        val = val * base + (idx - alphabet);
        str++;
    }

    if (endptr)
        *endptr = (char *)str;
    if (neg)
        return -val;
    else
        return val;
}

long long wcstoll(const wchar_t *str, wchar_t **endptr, int base)
{
    int c, neg = 0;
    const wchar_t *alphabet = L"0123456789abcdefghijklmnopqrstuvwxyz";
    long long val = 0;

    if (base != 0 && (base < 2 || base > 36)) return 0;

    /* skip whitespace */
    while (*str && isspace(*str))
        str++;

    /* parse potential -/+ (default to positive) */
    if (*str == '-')
    {
        neg = 1;
        str++;
    }
    else if (*str == '+')
    {
        str++;
    }

    /* parse prefix (0x, 0) */
    if (base == 0 || base == 16)
    {
        if (str[0] == '0' && str[1] == 'x')
        {
            base = 16;
            str += 2;
        }
    }
    if (base == 0)
    {
        if (str[0] == '0')
            base = 8;
    }
    if (base == 0)
        base = 10;

    /* parse number */
    while ((c = *str))
    {
        /* search for c */
        const wchar_t *idx = (const wchar_t*)memchr(alphabet, tolower(c), base);
        if (idx == 0)
            break;
        /* XXX clamp to LONG_MAX / LONG_MIN */
        val = val * base + (idx - alphabet);
        str++;
    }

    if (endptr)
        *endptr = (wchar_t *)str;
    if (neg)
        return -val;
    else
        return val;
}

unsigned long long strtoull(const char *str, char **endptr, int base)
{
    return (unsigned long long)strtoll(str, endptr, base);
}

long strtol(const char *str, char **endptr, int base)
{
    return (long)strtoll(str, endptr, base);
}

unsigned long long wcstoull(const wchar_t *str, wchar_t **endptr, int base)
{
    return (unsigned long long)wcstoll(str, endptr, base);
}

long wcstol(const wchar_t *str, wchar_t **endptr, int base)
{
    return (long)wcstoll(str, endptr, base);
}

unsigned long wcstoul(const wchar_t *str, wchar_t **endptr, int base)
{
    return (unsigned long)wcstoll(str, endptr, base);
}

unsigned long strtoul(const char *str, char **endptr, int base)
{
    return (unsigned long)strtoll(str, endptr, base);
}

float strtof(const char* str, char** endptr)
{
    return (float)strtold(str, endptr);
}

double strtod(const char* str, char** endptr)
{
    return (double)strtold(str, endptr);
}

float wcstof(const wchar_t* str, wchar_t** endptr)
{
    return (float)wcstold(str, endptr);
}

double wcstod(const wchar_t* str, wchar_t** endptr)
{
    return (double)wcstold(str, endptr);
}
