/*
vector.hpp

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
#ifndef VECTOR_HPP
#define VECTOR_HPP

#include <stdint.h>

template <typename T>
class vector
{
public:
    using iterator = T*;

    vector()
    {
        reserve(16);
    }

    explicit vector(size_t size, T val = T())
    {
        reserve(16);
        resize(size);
        fill(val);
    }

    vector(const vector<T>& other)
    {
        *this = other;
    }

    ~vector()
    {
        delete[] m_base;
    }

public:
    size_t size() const { return m_size; }
    size_t capacity() const { return m_capacity; }
    bool empty() const { return size() == 0; }

    vector<T>& operator=(const vector<T>& other)
    {
        resize(other.size());
        for (size_t i { 0 }; i < size(); ++i)
        {
            (*this)[i] = other[i];
        }

        return *this;
    }

    T* data()
    {
        return &m_base[0];
    }

    iterator begin() const
    {
        return &m_base[0];
    }
    iterator end() const
    {
        return &m_base[size()];
    }

    T& front()
    {
        return (*this)[0];
    }
    const T& front() const
    {
        return (*this)[0];
    }
    T& back()
    {
        return (*this)[size()-1];
    }
    const T& back() const
    {
        return (*this)[size()-1];
    }

    T& operator[](size_t n)
    {
        return m_base[n];
    }

    const T& operator[](size_t n) const
    {
        return m_base[n];
    }

    void clear()
    {
        resize(0);
    }

    void push_back(const T& val)
    {
        resize(size()+1);
        back() = val;
    }

    void pop_back()
    {
        resize(size()-1);
    }

    void fill(const T& val)
    {
        for (size_t i { 0 }; i < size(); ++i)
        {
            (*this)[i] = val;
        }
    }

    void resize(size_t n)
    {
        if (capacity() < n)
        {
            realloc(2 * n);
        }
        m_size = n;
    }

    void reserve(size_t n)
    {
        if (n > capacity())
        {
            realloc(n);
        }
    }

    void shrink_to_fit()
    {
        realloc(size());
    }

    vector<T>& operator+=(const vector<T>& other)
    {
        *this = *this + other;
        return *this;
    }

    vector<T>& operator+=(const T& other)
    {
        push_back(other);
        return *this;
    }

private:
    void realloc(size_t n)
    {
        m_capacity = n;

        T* newbuf = new T[n];
        for (size_t i { 0 }; i < size(); ++i)
        {
            newbuf[i] = (*this)[i];
        }

        delete[] m_base;

        m_base = newbuf;
    }

private:
    T* m_base { nullptr };
    size_t m_capacity { 0 };
    size_t m_size { 0 };
};


template <typename T>
vector<T> operator+(const vector<T>& lhs, const vector<T>& rhs)
{
    vector<T> result = lhs;
    for (const auto& el : rhs)
    {
        result.push_back(el);
    }

    return result;
}

#endif // VECTOR_HPP
