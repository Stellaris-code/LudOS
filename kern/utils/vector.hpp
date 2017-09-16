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

#include <assert.h>

#include <stdint.h>

template <class _Tp>
struct vector
{
public:
    // types:
    typedef vector __self;
    typedef _Tp                                   value_type;
    typedef value_type&                           reference;
    typedef const value_type&                     const_reference;
    typedef value_type*                           iterator;
    typedef const value_type*                     const_iterator;
    typedef value_type*                           pointer;
    typedef const value_type*                     const_pointer;
    typedef size_t                                size_type;
    typedef ptrdiff_t                             difference_type;

private:
    size_t                  __size_;
    value_type *            __base_;
    inline vector() noexcept :  __size_(0), __base_(nullptr) {}

    static inline value_type* __allocate (size_t count)
    {
        return new _Tp[count];
    }

    static inline void __deallocate (value_type* val) noexcept
    {
        delete[] val;
    }

public:

    explicit vector(size_type __c);
    vector(size_type __c, const value_type& __v);
    vector(const vector& __d);

    vector& operator=(const vector&) = delete;
    ~vector();

    // capacity:
    inline size_type size()     const noexcept { return __size_; }
    inline size_type max_size() const noexcept { return __size_; }
    inline bool      empty()    const noexcept { return __size_ == 0; }

    // element access:
    inline reference       operator[](size_type __n)       { return data()[__n]; }
    inline const_reference operator[](size_type __n) const { return data()[__n]; }

    inline reference       front()       { return data()[0]; }
    inline const_reference front() const { return data()[0]; }
    inline reference       back()        { return data()[__size_-1]; }
    inline const_reference back()  const { return data()[__size_-1]; }

    inline const_reference at(size_type __n) const;
    inline reference       at(size_type __n);

    // data access:
    inline _Tp*       data()       noexcept { return __base_; }
    inline const _Tp* data() const noexcept { return __base_; }
};

template <class _Tp>
inline
vector<_Tp>::vector(size_type __c) : vector ()
{
    __base_ = __allocate (__c);
    __size_ = __c;
    value_type *__data = data ();
    for ( __size_ = 0; __size_ < __c; ++__size_, ++__data )
        ::new (__data) value_type;
}

template <class _Tp>
inline
vector<_Tp>::vector(size_type __c, const value_type& __v) : vector ()
{
    __base_ = __allocate (__c);
    __size_ = __c;
    value_type *__data = data ();
    for ( __size_ = 0; __size_ < __c; ++__size_, ++__data )
        ::new (__data) value_type (__v);
}

template <class _Tp>
inline
vector<_Tp>::vector(const vector& __d) : vector ()
{
    size_t sz = __d.size();
    __base_ = __allocate (sz);
    value_type *__data = data ();
    auto src = __d.begin();
    for ( __size_ = 0; __size_ < sz; ++__size_, ++__data, ++src )
        ::new (__data) value_type (*src);
}

template <class _Tp>
inline
vector<_Tp>::~vector()
{
    value_type *__data = data () + __size_;
    for ( size_t i = 0; i < __size_; ++i )
        (--__data)->value_type::~value_type();
    __deallocate ( __base_ );
}

template <class _Tp>
inline
typename vector<_Tp>::reference
vector<_Tp>::at(size_type __n)
{
    assert_msg(__n < __size_, "vector::at out_of_range");
    return data()[__n];
}

template <class _Tp>
inline
typename vector<_Tp>::const_reference
vector<_Tp>::at(size_type __n) const
{
    assert_msg(__n < __size_, "vector::at out_of_range");
    return data()[__n];
}

#endif // VECTOR_HPP
