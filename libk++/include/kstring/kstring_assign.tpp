/*
kpp::string_assign.tpp

Copyright (c) 19 Yann BOUCHER (yann)

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

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize>::kernel_string()
{
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize>::kernel_string(const kernel_string &str)
{
    assign(str);
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize>::kernel_string(const kernel_string &str, size_t pos, size_t len)
{
    assign(str, pos, len);
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize>::kernel_string(const CharType *str, size_t n)
{
    assign(str, n);
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize>::kernel_string(size_t n, CharType c)
{
    assign(n, c);
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize>::kernel_string(kernel_string &&str)
{
    assign(static_cast<kernel_string&&>(str));
}

template<typename CharType, size_t SmallStrSize>
template<class InputIterator>
kernel_string<CharType, SmallStrSize>::kernel_string(InputIterator first, InputIterator last)
{
    assign(first, last);
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize>::kernel_string(const CharType *str)
    : kernel_string(str, string_length(str))
{
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize>::~kernel_string()
{
    if (m_heap_str) release_heap();
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> &kernel_string<CharType, SmallStrSize>::operator=(const kernel_string &str)
{
    return assign(str);
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> &kernel_string<CharType, SmallStrSize>::operator=(const CharType *str)
{
    return assign(str);
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> &kernel_string<CharType, SmallStrSize>::operator=(CharType c)
{
    resize(1, c);

    return *this;
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> &kernel_string<CharType, SmallStrSize>::operator=(kernel_string &&str)
{
    return assign(static_cast<kernel_string&&>(str));
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> &kernel_string<CharType, SmallStrSize>::assign(kernel_string &&str)
{
    // reap heap ptr
    if (str.uses_heap())
    {
        m_heap_str = str.m_heap_str;
        m_allocated = str.m_allocated;
        m_size = str.m_size;

        str.m_size = str.m_allocated = 0;
        str.m_heap_str = nullptr;
    }
    else
    {
        resize(str.size());
        memcpy(begin(), str.data(), str.size()*sizeof(CharType));
    }

    return *this;
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> &kernel_string<CharType, SmallStrSize>::assign(size_t n, CharType c)
{
    resize(n, c);

    return *this;
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> &kernel_string<CharType, SmallStrSize>::assign(const CharType *str, size_t n)
{
    resize(n);
    memcpy(begin(), str, n*sizeof(CharType));

    return *this;
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> &kernel_string<CharType, SmallStrSize>::assign(const CharType *str)
{
    return assign(str, string_length(str));
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> &kernel_string<CharType, SmallStrSize>::assign(const kernel_string &str, size_t pos, size_t len)
{
    assert(pos < str.size());

    if (len > str.size() - pos) len = str.size() - pos;

    resize(len);
    memcpy(begin(), str.data() + pos, len*sizeof(CharType));

    return *this;
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> &kernel_string<CharType, SmallStrSize>::assign(const kernel_string &str)
{
    resize(str.size());

    memcpy(begin(), str.data(), str.size()*sizeof(CharType));

    return *this;
}

template<typename CharType, size_t SmallStrSize>
template<class InputIterator>
kernel_string<CharType, SmallStrSize> &kernel_string<CharType, SmallStrSize>::assign(InputIterator first, InputIterator last)
{
    auto size = last - first;
    resize(size);
    for (size_t i { 0 }; i < size; ++i)
    {
        (*this)[i] = *first;
        ++first;
    }

    return *this;
}
