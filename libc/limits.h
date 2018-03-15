/*
limits.h

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
#ifndef L_LIMITS_H
#define L_LIMITS_H

#include <features.h>


/* Maximum length of any multibyte character in any locale.
   We define this value here since the gcc header does not define
   the correct value.  */
#define MB_LEN_MAX	16


#include <bits/wordsize.h>

/* We don't have #include_next.
   Define ANSI <limits.h> for standard 32-bit words.  */

/* These assume 8-bit `char's, 16-bit `short int's,
   and 32-bit `int's and `long int's.  */

/* Number of bits in a `char'.	*/
#  define CHAR_BIT	8

/* Minimum and maximum values a `signed char' can hold.  */
#  define SCHAR_MIN	(-128)
#  define SCHAR_MAX	127

/* Maximum value an `unsigned char' can hold.  (Minimum is 0.)  */
#  define UCHAR_MAX	255

/* Minimum and maximum values a `char' can hold.  */
#  ifdef __CHAR_UNSIGNED__
#   define CHAR_MIN	0
#   define CHAR_MAX	UCHAR_MAX
#  else
#   define CHAR_MIN	SCHAR_MIN
#   define CHAR_MAX	SCHAR_MAX
#  endif

/* Minimum and maximum values a `signed short int' can hold.  */
#  define SHRT_MIN	(-32768)
#  define SHRT_MAX	32767

/* Maximum value an `unsigned short int' can hold.  (Minimum is 0.)  */
#  define USHRT_MAX	65535

/* Minimum and maximum values a `signed int' can hold.  */
#  define INT_MIN	(-INT_MAX - 1)
#  define INT_MAX	2147483647

/* Maximum value an `unsigned int' can hold.  (Minimum is 0.)  */
#  define UINT_MAX	4294967295U

/* Minimum and maximum values a `signed long int' can hold.  */
#  if __WORDSIZE == 64
#   define LONG_MAX	9223372036854775807L
#  else
#   define LONG_MAX	2147483647L
#  endif
#  define LONG_MIN	(-LONG_MAX - 1L)

/* Maximum value an `unsigned long int' can hold.  (Minimum is 0.)  */
#  if __WORDSIZE == 64
#   define ULONG_MAX	18446744073709551615UL
#  else
#   define ULONG_MAX	4294967295UL
#  endif

#  ifdef __USE_ISOC99

/* Minimum and maximum values a `signed long long int' can hold.  */
#   define LLONG_MAX	9223372036854775807LL
#   define LLONG_MIN	(-LLONG_MAX - 1LL)

/* Maximum value an `unsigned long long int' can hold.  (Minimum is 0.)  */
#   define ULLONG_MAX	18446744073709551615ULL

#  endif /* ISO C99 */

/* The <limits.h> files in some gcc versions don't define LLONG_MIN,
   LLONG_MAX, and ULLONG_MAX.  Instead only the values gcc defined for
   ages are available.  */
#if defined __USE_ISOC99 && defined __GNUC__
# ifndef LLONG_MIN
#  define LLONG_MIN	(-LLONG_MAX-1)
# endif
# ifndef LLONG_MAX
#  define LLONG_MAX	__LONG_LONG_MAX__
# endif
# ifndef ULLONG_MAX
#  define ULLONG_MAX	(LLONG_MAX * 2ULL + 1)
# endif
#endif


#endif // LIMITS_H
