/*
kpp::string.hpp

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
#ifndef KSTRING_HPP
#define KSTRING_HPP

#include <stddef.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

namespace kpp
{

template <typename CharType = char, size_t SmallStrSize = 60>
class kernel_string
{
public:
    using value_type = CharType;
    using reference = CharType&;
    using const_reference = CharType&;
    using pointer = CharType*;
    using const_pointer = CharType*;
    using iterator = CharType*;
    using const_iterator = CharType*;
    using difference_type = ptrdiff_t;
    using size_type = size_t;

    static constexpr size_t npos = -1;

public:
    kernel_string();
    kernel_string(const kernel_string& str);
    kernel_string(const kernel_string& str, size_t pos, size_t len = npos);
    kernel_string(const CharType* str);
    kernel_string(const CharType* str, size_t n);
    kernel_string(size_t n, CharType c);
    template <class InputIterator>
    kernel_string(InputIterator first, InputIterator last);
    kernel_string(kernel_string&& str) noexcept;
    ~kernel_string();

    kernel_string& operator=(const kernel_string& str);
    kernel_string& operator=(const CharType* str);
    kernel_string& operator=(CharType c);
    kernel_string& operator=(kernel_string&& str) noexcept;

    kernel_string& assign(const kernel_string& str);
    kernel_string& assign(const kernel_string& str, size_t pos, size_t len = npos);
    kernel_string& assign(const CharType* str);
    kernel_string& assign(const CharType* str, size_t n);
    kernel_string& assign(size_t n, CharType c);
    template <class InputIterator>
    kernel_string& assign(InputIterator first, InputIterator last);
    kernel_string& assign(kernel_string&& str);

    void resize(size_t n) noexcept { resize(n, CharType{}); };
    void resize(size_t n, CharType c) noexcept;
    void clear() noexcept;

    kernel_string& append(const kernel_string& str);
    kernel_string& append(const CharType* str, size_t n);
    kernel_string& append(const CharType* str);
    kernel_string& append(size_t n, CharType c);

    void push_back(CharType c);
    void pop_back();

    kernel_string& operator+=(const kernel_string& str);
    kernel_string& operator+=(const CharType* str);
    kernel_string& operator+=(CharType c);

    kernel_string substr(size_t pos = 0, size_t len = npos) const;

    size_t find(const kernel_string& str, size_t pos = 0) const noexcept;
    size_t find(const CharType* str, size_t pos = 0) const noexcept;
    size_t find(const CharType* str, size_t pos, size_t n) const noexcept;
    size_t find(CharType c, size_t pos = 0) const noexcept;

    size_t find_first_of(const kernel_string& str, size_t pos = 0) const noexcept;


    kernel_string& erase(size_t pos = 0, size_t len = npos);
    iterator erase(const_iterator p);
    iterator erase(const_iterator start, const_iterator end);

    size_t size() const noexcept { return m_size; }
    size_t length() const noexcept { return size(); }
    bool empty() const noexcept { return size() == 0; }
    size_t capacity() const noexcept { return m_allocated; }

    size_t uses_heap() const noexcept { return m_size > SmallStrSize; }

    const CharType* c_str() const noexcept;
    const CharType* data() const noexcept { return c_str(); }

    iterator begin() const noexcept { return const_cast<CharType*>(c_str()); }
    iterator end() const noexcept { return const_cast<CharType*>(c_str() + size()); }

    CharType& front() { return (*this)[0]; }
    const CharType& front() const { return (*this)[0]; }
    CharType& back() { return (*this)[size() - 1]; }
    const CharType& back() const { return (*this)[size() - 1]; }

    CharType& operator[] (size_t pos) { assert(pos < size()); return mutable_data()[pos]; }
    const CharType& operator[] (size_t pos) const { assert(pos < size());  return data()[pos]; }

private:
    CharType* mutable_data() noexcept { return const_cast<CharType*>(c_str()); }
    void allocate_heap(size_t size);
    void release_heap();

    static size_t string_length(const CharType* c);

private:
    CharType m_stack_str[SmallStrSize+1] = { CharType{} }; // reserve one char for the null terminator
    size_t m_size { 0 };
    size_t m_allocated { 0 };
    CharType* m_heap_str { nullptr };
};

template <typename CharType = char, size_t SmallStrSize = 60>
kernel_string<CharType, SmallStrSize> operator+(const kernel_string<CharType, SmallStrSize>& lhs, const kernel_string<CharType, SmallStrSize>& rhs);
template <typename CharType = char, size_t SmallStrSize = 60>
kernel_string<CharType, SmallStrSize> operator+(const CharType* lhs, const kernel_string<CharType, SmallStrSize>& rhs);
template <typename CharType = char, size_t SmallStrSize = 60>
kernel_string<CharType, SmallStrSize> operator+(const kernel_string<CharType, SmallStrSize>& lhs, const CharType* rhs);
template <typename CharType = char, size_t SmallStrSize = 60>
kernel_string<CharType, SmallStrSize> operator+(CharType lhs, const kernel_string<CharType, SmallStrSize>& rhs);
template <typename CharType = char, size_t SmallStrSize = 60>
kernel_string<CharType, SmallStrSize> operator+(const kernel_string<CharType, SmallStrSize>& lhs, CharType rhs);

template <typename CharType = char, size_t SmallStrSize = 60>
bool operator== (const kernel_string<CharType, SmallStrSize>& lhs, const kernel_string<CharType, SmallStrSize>& rhs) noexcept;
template <typename CharType = char, size_t SmallStrSize = 60>
bool operator== (const char*   lhs, const kernel_string<CharType, SmallStrSize>& rhs);
template <typename CharType = char, size_t SmallStrSize = 60>
bool operator== (const kernel_string<CharType, SmallStrSize>& lhs, const char*   rhs);

template <typename CharType = char, size_t SmallStrSize = 60>
bool operator!= (const kernel_string<CharType, SmallStrSize>& lhs, const kernel_string<CharType, SmallStrSize>& rhs) noexcept
{ return !(lhs == rhs); }
template <typename CharType = char, size_t SmallStrSize = 60>
bool operator!= (const char*   lhs, const kernel_string<CharType, SmallStrSize>& rhs)
{ return !(lhs == rhs); }
template <typename CharType = char, size_t SmallStrSize = 60>
bool operator!= (const kernel_string<CharType, SmallStrSize>& lhs, const char*   rhs)
{ return !(lhs == rhs); }

template <typename CharType = char, size_t SmallStrSize = 60>
bool operator< (const kernel_string<CharType, SmallStrSize>& lhs, const kernel_string<CharType, SmallStrSize>& rhs) noexcept;
template <typename CharType = char, size_t SmallStrSize = 60>
bool operator< (const char*   lhs, const kernel_string<CharType, SmallStrSize>& rhs);
template <typename CharType = char, size_t SmallStrSize = 60>
bool operator< (const kernel_string<CharType, SmallStrSize>& lhs, const char*   rhs);

template <typename ConvType, typename CharType = char, size_t SmallStrSize = 60>
kernel_string<CharType, SmallStrSize> do_string_conversion(ConvType val, const char* fmt)
{
    char buffer[SmallStrSize];
    size_t len = ksnprintf(buffer, sizeof(buffer), fmt, val);

    assert(len >= 0 && len <= sizeof(buffer));

    return kernel_string<CharType, SmallStrSize>{buffer};
}

template <typename CharType = char, size_t SmallStrSize = 60>
kernel_string<CharType, SmallStrSize> to_string(int v)
{
    return do_string_conversion<int, CharType, SmallStrSize>(v, "%d");
}
template <typename CharType = char, size_t SmallStrSize = 60>
kernel_string<CharType, SmallStrSize> to_string(long v)
{
    return do_string_conversion<long, CharType, SmallStrSize>(v, "%l");
}
template <typename CharType = char, size_t SmallStrSize = 60>
kernel_string<CharType, SmallStrSize> to_string(unsigned v)
{
    return do_string_conversion<unsigned, CharType, SmallStrSize>(v, "%u");
}
template <typename CharType = char, size_t SmallStrSize = 60>
kernel_string<CharType, SmallStrSize> to_string(unsigned long v)
{
    return do_string_conversion<unsigned long, CharType, SmallStrSize>(v, "%u");
}
template <typename CharType = char, size_t SmallStrSize = 60>
kernel_string<CharType, SmallStrSize> to_string(float v)
{
    return do_string_conversion<float, CharType, SmallStrSize>(v, "%f");
}
template <typename CharType = char, size_t SmallStrSize = 60>
kernel_string<CharType, SmallStrSize> to_string(double v)
{
    return do_string_conversion<double, CharType, SmallStrSize>(v, "%f");
}
template <typename CharType = char, size_t SmallStrSize = 60>
kernel_string<CharType, SmallStrSize> to_hex_string(unsigned v)
{
    return do_string_conversion<unsigned, CharType, SmallStrSize>(v, "%x");
}
// TODO : use kpp::expected
template <typename CharType = char, size_t SmallStrSize = 60>
unsigned long stoul(const kernel_string<CharType, SmallStrSize>& str)
{
    char* end = nullptr;
    unsigned long result = strtoul(str.c_str(), &end, 0);
    assert(end != nullptr);

    return result;
}

using string = kernel_string<>;
using u16string = kernel_string<char16_t>;
using u32string = kernel_string<char32_t>;

template <typename CharType = char, size_t SmallStrSize = 60>
unsigned long khash(const kernel_string<CharType, SmallStrSize>& str)
{
    unsigned long hash = 5381;
    int c;

    const CharType* ptr = str.c_str();

    while ((c = *ptr++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

#include "kstring.tpp"
#include "kstring_assign.tpp"

}

namespace std
{
inline namespace __1
{
template <typename T>
struct hash;
}

template <typename CharType, size_t SmallStrSize>
struct __1::hash<kpp::kernel_string<CharType, SmallStrSize>>
{
public:
    size_t operator()(const kpp::kernel_string<CharType, SmallStrSize> &str) const
    {
        return kpp::khash(str);
    }
};
}

#endif // KSTRING_HPP
