/*
ringbuf.hpp

Copyright (c) 29 Yann BOUCHER (yann)

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
#ifndef RINGBUF_HPP
#define RINGBUF_HPP

#include "utils/array.hpp"
#include "utils/swap.hpp"

#include <stdint.h>
#include <string.h>
#include <assert.h>

template <typename T, size_t Size>
class CircularBuffer
{
public:
    typedef size_t size_type;
    typedef T& reference;
    typedef const T& const_reference;

    explicit CircularBuffer();

    CircularBuffer<T, Size>& operator=(CircularBuffer<T, Size> rhs);

    size_type size() const
    {
        if (_full)
        {
            return Size;
        }
        else
        {
            return _front;
        }
    }
    bool is_full() const { return _full; }

    const_reference operator[](size_type index) const;
    reference operator[](size_type index);

    void add(const T &item);

    friend void swap(CircularBuffer<T, Size> &a, CircularBuffer<T, Size> &b)
    {
        swap(a._buffer, b._buffer);
        swap(a._capacity, b._capacity);
        swap(a._front, b._front);
        swap(a._full, b._full);
    }

private:
    T _buffer[Size];
    size_type _front;
    bool _full;
};

template<typename T, size_t Size>
CircularBuffer<T, Size>::CircularBuffer()
    : _front(0)
    , _full(false)
{
}

template<typename T, size_t Size>
typename CircularBuffer<T, Size>::const_reference
CircularBuffer<T, Size>::operator[](size_type index) const
{
    if (_full)
    {
        assert_msg(index < size(), "Invalid access of circular buffer %p at index %d !", this, index);

        return _buffer[(_front + index) % size()];
    }
    else
    {
        assert_msg(index < size(), "Invalid access of circular buffer %p at index %d with _front %d !", this, index, _front);
        return _buffer[index];
    }
}

template<typename T, size_t Size>
typename CircularBuffer<T, Size>::reference
CircularBuffer<T, Size>::operator[](size_type index)
{
    return const_cast<reference>(static_cast<const CircularBuffer<T, Size>&>(*this)[index]);
}

template<typename T, size_t Size>
CircularBuffer<T, Size>&
CircularBuffer<T, Size>::operator=(CircularBuffer<T, Size> rhs)
{
    swap(*this, rhs);
    return *this;
}

template<typename T, size_t Size>
void
CircularBuffer<T, Size>::add(const T& item)
{
    memcpy(_buffer[_front++], item, ::size(item) * sizeof(T));
    if (_front == Size)
    {
        _front = 0;
        _full = true;
    }
}

#endif // RINGBUF_HPP
