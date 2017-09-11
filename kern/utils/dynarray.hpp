// -*- C++ -*-
//===-------------------------- dynarray ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_DYNARRAY
#define _LIBCPP_DYNARRAY

#include <limits.h>
#include <stdint.h>
#include <assert.h>
#include <new.hpp>

template <class _Tp>
struct dynarray
{
public:
    // types:
    typedef dynarray __self;
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
    inline dynarray() noexcept :  __size_(0), __base_(nullptr) {}

    static inline value_type* __allocate (size_t count)
    {
        if (__SIZE_MAX__ / sizeof (value_type) <= count)
        {
            ::impl_assert_msg(true, "d", "d",4,"d","dynarray::allocation");
        }
        return static_cast<value_type *> (__builtin_alloca(sizeof(value_type) * count));
    }

    static inline void __deallocate ( value_type* __ptr ) noexcept
    {

    }

public:

    explicit dynarray(size_type __c);
    dynarray(size_type __c, const value_type& __v);
    dynarray(const dynarray& __d);

//  We're not implementing these right now.
//  Updated with the resolution of LWG issue #2255
//     template <typename _Alloc>
//       dynarray(allocator_arg_t, const _Alloc& __alloc, size_type __c);
//     template <typename _Alloc>
//       dynarray(allocator_arg_t, const _Alloc& __alloc, size_type __c, const value_type& __v);
//     template <typename _Alloc>
//       dynarray(allocator_arg_t, const _Alloc& __alloc, const dynarray& __d);
//     template <typename _Alloc>
//       dynarray(allocator_arg_t, const _Alloc& __alloc, initializer_list<value_type>);

    dynarray& operator=(const dynarray&) = delete;
    ~dynarray();

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
dynarray<_Tp>::dynarray(size_type __c) : dynarray ()
{
    __base_ = __allocate (__c);
    value_type *__data = data ();
    for ( __size_ = 0; __size_ < __c; ++__size_, ++__data )
        ::new (__data) value_type;
}

template <class _Tp>
inline
dynarray<_Tp>::dynarray(size_type __c, const value_type& __v) : dynarray ()
{
    __base_ = __allocate (__c);
    value_type *__data = data ();
    for ( __size_ = 0; __size_ < __c; ++__size_, ++__data )
        ::new (__data) value_type (__v);
}

template <class _Tp>
inline
dynarray<_Tp>::dynarray(const dynarray& __d) : dynarray ()
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
dynarray<_Tp>::~dynarray()
{
    value_type *__data = data () + __size_;
    for ( size_t i = 0; i < __size_; ++i )
        (--__data)->value_type::~value_type();
    __deallocate ( __base_ );
}

template <class _Tp>
inline
typename dynarray<_Tp>::reference
dynarray<_Tp>::at(size_type __n)
{
    if (__n >= __size_)
    {
        assert_msg(true, "dynarray::at out_of_range");
    }
    return data()[__n];
}

template <class _Tp>
inline
typename dynarray<_Tp>::const_reference
dynarray<_Tp>::at(size_type __n) const
{
    if (__n >= __size_)
    {
        assert_msg(true, "dynarray::at out_of_range");
    }
    return data()[__n];
}

#endif  // _LIBCPP_DYNARRAY
