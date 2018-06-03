/*
kpp::string.cpp

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

template<typename CharType, size_t SmallStrSize>
void kernel_string<CharType, SmallStrSize>::resize(size_t n, CharType c) noexcept
{
    size_t old_size = m_size;
    // copy the old string
    CharType old_str[old_size?:1]; // VLAs cannot be empty
    memcpy(old_str, data(), old_size*sizeof(CharType));

    if (n > SmallStrSize) // using a heap allocated str
    {
        // if we have to allocate a larger heap array
        if (n > m_allocated)
        {
            allocate_heap(n);
        }
        // else, we can keep our array
    }
    else
    {
        // switch to the stack array
        if (uses_heap())
        {
            assert(m_heap_str);
            release_heap();
        }

        m_allocated = n;
    }

    m_size = n;

    // fill the array with c
    for (size_t i { 0 }; i < size(); ++i)
    {
        mutable_data()[i] = c;
    }
    // copy the old data to the new array
    memcpy(begin(), old_str, (size() > old_size ? old_size : size())*sizeof(CharType));

    // put the null terminator
    mutable_data()[size()] = '\0';
}

template<typename CharType, size_t SmallStrSize>
void kernel_string<CharType, SmallStrSize>::clear() noexcept
{
    resize(0);
}

template<typename CharType, size_t SmallStrSize>
void kernel_string<CharType, SmallStrSize>::push_back(CharType c)
{
    *this += c;
}

template<typename CharType, size_t SmallStrSize>
void kernel_string<CharType, SmallStrSize>::pop_back()
{
    resize(size()-1);
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> &kernel_string<CharType, SmallStrSize>::append(size_t n, CharType c)
{
    return append(kernel_string(n, c));
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> &kernel_string<CharType, SmallStrSize>::append(const CharType *str, size_t n)
{
    size_t old_size = size();
    resize(size() + n);
    memcpy(mutable_data() + old_size, str, n * sizeof(CharType));

    return *this;
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> &kernel_string<CharType, SmallStrSize>::append(const CharType *str)
{
    return append(str, string_length(str));
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> &kernel_string<CharType, SmallStrSize>::append(const kernel_string &str)
{
    return append(str.c_str());
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> &kernel_string<CharType, SmallStrSize>::operator+=(const kernel_string &str)
{
    return append(str);
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> &kernel_string<CharType, SmallStrSize>::operator+=(const CharType* str)
{
    return append(str);
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> &kernel_string<CharType, SmallStrSize>::operator+=(CharType c)
{
    return append(1, c);
}

template<typename CharType, size_t SmallStrSize>
size_t kernel_string<CharType, SmallStrSize>::find_first_of(const kernel_string &str, size_t pos) const noexcept
{
    for (size_t i { pos }; i < size(); ++i)
    {
        for (auto c : str)
        {
            if ((*this)[i] == c) return i;
        }
    }

    return npos;
}

template<typename CharType, size_t SmallStrSize>
size_t kernel_string<CharType, SmallStrSize>::find(const kernel_string &str, size_t pos) const noexcept
{
    if (str.empty()) return npos;

    for (size_t i { pos }; i <= size() - str.size(); ++i)
    {
        if (substr(i, str.size()) == str)
        {
            return i;
        }
    }

    return npos;
}

template<typename CharType, size_t SmallStrSize>
size_t kernel_string<CharType, SmallStrSize>::find(CharType c, size_t pos) const noexcept
{
    for (size_t i { pos }; i < size(); ++i)
    {
        if ((*this)[i] == c)
        {
            return i;
        }
    }

    return npos;
}

template<typename CharType, size_t SmallStrSize>
size_t kernel_string<CharType, SmallStrSize>::find(const CharType *str, size_t pos) const noexcept
{
    return find(kernel_string(str), pos);
}

template<typename CharType, size_t SmallStrSize>
size_t kernel_string<CharType, SmallStrSize>::find(const CharType *str, size_t pos, size_t n) const noexcept
{
    return find(kernel_string(str, n), pos);
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> kernel_string<CharType, SmallStrSize>::substr(size_t pos, size_t len) const
{
    return kpp::string(*this, pos, len);
}

template<typename CharType, size_t SmallStrSize>
typename kernel_string<CharType, SmallStrSize>::iterator kernel_string<CharType, SmallStrSize>::erase(const_iterator p)
{
    kpp::string new_str;
    size_t idx = p - begin();

    new_str = substr(0, idx);
    new_str += substr(idx + 1);

    *this = new_str;

    return idx < size() ? &(*this)[idx] : end();
}

template<typename CharType, size_t SmallStrSize>
typename kernel_string<CharType, SmallStrSize>::iterator kernel_string<CharType, SmallStrSize>::erase
(kernel_string::const_iterator start, kernel_string::const_iterator end)
{
    kpp::string copy = *this;

    size_t start_idx = start - begin();
    size_t end_idx = end - begin();

    copy.erase(start_idx, end_idx - start_idx);

    *this = copy;

    return start_idx < size() ? &(*this)[start_idx] : this->end();
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> &kernel_string<CharType, SmallStrSize>::erase(size_t pos, size_t len)
{
    kpp::string new_str;

    new_str = substr(0, pos);
    if (len != npos && pos + len < size()) new_str += substr(pos + len);

    return *this = new_str;
}

template<typename CharType, size_t SmallStrSize>
const CharType* kernel_string<CharType, SmallStrSize>::c_str() const noexcept
{
    if (uses_heap())
    {
        assert(m_heap_str);
        return m_heap_str;
    }
    else
    {
        return m_stack_str;
    }
}

template<typename CharType, size_t SmallStrSize>
void kernel_string<CharType, SmallStrSize>::allocate_heap(size_t size)
{
    if (m_heap_str) delete[] m_heap_str;
    m_heap_str = new CharType[size+1];
    m_allocated = size;

    m_heap_str[size] = CharType{}; // null terminator
}

template<typename CharType, size_t SmallStrSize>
void kernel_string<CharType, SmallStrSize>::release_heap()
{
    delete[] m_heap_str;
    m_heap_str = nullptr;
}

template<typename CharType, size_t SmallStrSize>
size_t kernel_string<CharType, SmallStrSize>::string_length(const CharType *c)
{
    size_t len = 0;
    while (c[len] != CharType{})
    {
        ++len;
    }

    return len;
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> operator+(const kernel_string<CharType, SmallStrSize> &lhs, const kernel_string<CharType, SmallStrSize> &rhs)
{
    kernel_string<CharType, SmallStrSize> str;
    str += lhs;
    str += rhs;

    return str;
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> operator+(const CharType *lhs, const kernel_string<CharType, SmallStrSize> &rhs)
{
    return kernel_string<CharType, SmallStrSize>(lhs) + rhs;
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> operator+(CharType lhs, const kernel_string<CharType, SmallStrSize> &rhs)
{
    return kernel_string<CharType, SmallStrSize>(1, lhs) + rhs;
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> operator+(const kernel_string<CharType, SmallStrSize> &lhs, const CharType *rhs)
{
    return lhs + kernel_string<CharType, SmallStrSize>(rhs);
}

template<typename CharType, size_t SmallStrSize>
kernel_string<CharType, SmallStrSize> operator+(const kernel_string<CharType, SmallStrSize> &lhs, CharType rhs)
{
    return lhs + kernel_string<CharType, SmallStrSize>(1, rhs);
}

template<typename CharType, size_t SmallStrSize>
bool operator==(const kernel_string<CharType, SmallStrSize> &lhs, const kernel_string<CharType, SmallStrSize> &rhs) noexcept
{
    if (lhs.size() != rhs.size()) return false;

    for (size_t i { 0 }; i < lhs.size(); ++i)
    {
        if (lhs[i] != rhs[i]) return false;
    }

    return true;
}

template<typename CharType, size_t SmallStrSize>
bool operator==(const char *lhs, const kernel_string<CharType, SmallStrSize> &rhs)
{
    return kernel_string<CharType, SmallStrSize>(lhs) == rhs;
}

template<typename CharType, size_t SmallStrSize>
bool operator==(const kernel_string<CharType, SmallStrSize> &lhs, const char *rhs)
{
    return lhs == kernel_string<CharType, SmallStrSize>(rhs);
}

template<typename CharType, size_t SmallStrSize>
bool operator<(const kernel_string<CharType, SmallStrSize> &lhs, const kernel_string<CharType, SmallStrSize> &rhs) noexcept
{
    for (size_t i { 0 }; i < (lhs.size() < rhs.size() ? lhs.size() : rhs.size()); ++i)
    {
        if (lhs[i] < rhs[i]) return true;
        else if (lhs[i] > rhs[i]) return false;
    }

    if (lhs.size() < rhs.size()) return true;

    return false;
}
