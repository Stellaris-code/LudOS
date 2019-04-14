/*
mathutils.hpp

Copyright (c) 01 Yann BOUCHER (yann)

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
#ifndef MATHUTILS_HPP
#define MATHUTILS_HPP

#include <math.h>

#include <type_traits.hpp>

template <typename T>
T diff(T lhs, T rhs)
{
    return abs(std::make_signed_t<T>(lhs) - std::make_signed_t<T>(rhs));
}

template <typename T>
T round_to(T value, T roundTo)
{
    return (value + (roundTo - 1)) & ~(roundTo - 1);
}

inline int log2int(int value)
{
    int targetlevel = 0;
    while (value >>= 1) ++targetlevel;

    return targetlevel;
}

inline size_t intlog(size_t base, size_t x)
{
    assert(base > 0);
    assert(log2int(base) != 0);
    return static_cast<size_t>(log2int(x) / log2int(base));
}

template <typename T>
inline bool is_power_of(T value, T base)
{
    return log10(value) / log10(base) ==
            floor(log10(value) / log10(base));
}

template <typename T>
inline T ipow(T base, T exp)
{
    static_assert(std::is_integral_v<T>);

    T result = 1;
    while (exp)
    {
        if (exp & 1)
            result *= base;
        exp >>= 1;
        base *= base;
    }

    return result;
}

#endif // MATHUTILS_HPP
