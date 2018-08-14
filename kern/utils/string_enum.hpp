/*
string_enum.hpp

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
#ifndef STRING_ENUM_HPP
#define STRING_ENUM_HPP

#include <assert.h>
#include <array.hpp>

#define STRING_ENUM(name, ...) \
static constexpr const char name##list[] = #__VA_ARGS__; \
static constexpr kpp::array<char, sizeof(name##list)> name##transformed() \
{ \
    kpp::array<char, sizeof(name##list)> array {}; \
    for (size_t i = 0; i < sizeof(name##list); ++i) \
        array[i] = (name##list[i] == ',' ? '\0' : name##list[i]); \
    return array; \
} \
struct name \
{ \
    enum \
    { \
        __VA_ARGS__, \
        Max##name \
    }; \
    int enum_val = Max##name; \
    name() = default; \
    name(int val) \
{ enum_val = val; } \
    operator int() const { return enum_val; } \
    \
    static constexpr kpp::array<char, sizeof(name##list)> list = name##transformed(); \
    static constexpr const char* to_string(int val) \
    { \
        /*assert(val < Max##name);*/ \
        size_t offset = 0; \
        for (int i = 0; i < val; ++i) \
        { \
            while (list[offset] != '\0') \
                offset++; \
            offset += 2; /* skip both the null sep and the space */ \
        } \
        return list.data() + offset; \
    } \
};

#endif // STRING_ENUM_HPP
