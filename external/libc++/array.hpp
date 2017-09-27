// -*- C++ -*-
//===---------------------------- array -----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_ARRAY
#define _LIBCPP_ARRAY

#include <__config.hpp>
#include <__tuple.hpp>
#include <type_traits.hpp>
#include <utility.hpp>
#include <iterator.hpp>
#include <algorithm.hpp>
#include <stdexcept.hpp>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif



_LIBCPP_BEGIN_NAMESPACE_STD

template <class _Tp, size_t _Size>
struct _LIBCPP_TEMPLATE_VIS array
{
    // types:
    typedef array __self;
    typedef _Tp                                   value_type;
    typedef value_type&                           reference;
    typedef const value_type&                     const_reference;
    typedef value_type*                           iterator;
    typedef const value_type*                     const_iterator;
    typedef value_type*                           pointer;
    typedef const value_type*                     const_pointer;
    typedef size_t                                size_type;
    typedef ptrdiff_t                             difference_type;
    typedef std::reverse_iterator<iterator>       reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;

    value_type __elems_[_Size > 0 ? _Size : 1];

    // No explicit construct/copy/destroy for aggregate type
    _LIBCPP_INLINE_VISIBILITY void fill(const value_type& __u)
        {_VSTD::fill_n(__elems_, _Size, __u);}
    _LIBCPP_INLINE_VISIBILITY
    void swap(array& __a) _NOEXCEPT_(_Size == 0 || __is_nothrow_swappable<_Tp>::value)
        { __swap_dispatch((std::integral_constant<bool, _Size == 0>()), __a); }

    _LIBCPP_INLINE_VISIBILITY
    void __swap_dispatch(std::true_type, array&) {}

    _LIBCPP_INLINE_VISIBILITY
    void __swap_dispatch(std::false_type, array& __a)
        { _VSTD::swap_ranges(__elems_, __elems_ + _Size, __a.__elems_);}

    // iterators:
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
    iterator begin() _NOEXCEPT {return iterator(__elems_);}
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
    const_iterator begin() const _NOEXCEPT {return const_iterator(__elems_);}
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
    iterator end() _NOEXCEPT {return iterator(__elems_ + _Size);}
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
    const_iterator end() const _NOEXCEPT {return const_iterator(__elems_ + _Size);}

    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
    reverse_iterator rbegin() _NOEXCEPT {return reverse_iterator(end());}
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
    const_reverse_iterator rbegin() const _NOEXCEPT {return const_reverse_iterator(end());}
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
    reverse_iterator rend() _NOEXCEPT {return reverse_iterator(begin());}
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
    const_reverse_iterator rend() const _NOEXCEPT {return const_reverse_iterator(begin());}

    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
    const_iterator cbegin() const _NOEXCEPT {return begin();}
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
    const_iterator cend() const _NOEXCEPT {return end();}
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
    const_reverse_iterator crbegin() const _NOEXCEPT {return rbegin();}
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
    const_reverse_iterator crend() const _NOEXCEPT {return rend();}

    // capacity:
    _LIBCPP_INLINE_VISIBILITY
    _LIBCPP_CONSTEXPR size_type size() const _NOEXCEPT {return _Size;}
    _LIBCPP_INLINE_VISIBILITY
    _LIBCPP_CONSTEXPR size_type max_size() const _NOEXCEPT {return _Size;}
    _LIBCPP_INLINE_VISIBILITY
    _LIBCPP_CONSTEXPR bool empty() const _NOEXCEPT {return _Size == 0;}

    // element access:
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
    reference operator[](size_type __n)             {return __elems_[__n];}
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
    const_reference operator[](size_type __n) const {return __elems_[__n];}

    _LIBCPP_CONSTEXPR_AFTER_CXX14       reference at(size_type __n);
    _LIBCPP_CONSTEXPR_AFTER_CXX11 const_reference at(size_type __n) const;

    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14 reference front()             {return __elems_[0];}
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11 const_reference front() const {return __elems_[0];}
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14 reference back()              {return __elems_[_Size > 0 ? _Size-1 : 0];}
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11 const_reference back() const  {return __elems_[_Size > 0 ? _Size-1 : 0];}

    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
    value_type* data() _NOEXCEPT {return __elems_;}
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX14
    const value_type* data() const _NOEXCEPT {return __elems_;}
};

template <class _Tp, size_t _Size>
_LIBCPP_CONSTEXPR_AFTER_CXX14
typename array<_Tp, _Size>::reference
array<_Tp, _Size>::at(size_type __n)
{
    if (__n >= _Size)
        __throw_out_of_range("array::at");

    return __elems_[__n];
}

template <class _Tp, size_t _Size>
_LIBCPP_CONSTEXPR_AFTER_CXX11
typename array<_Tp, _Size>::const_reference
array<_Tp, _Size>::at(size_type __n) const
{
    if (__n >= _Size)
        __throw_out_of_range("array::at");
    return __elems_[__n];
}

template <class _Tp, size_t _Size>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator==(const array<_Tp, _Size>& __x, const array<_Tp, _Size>& __y)
{
    return _VSTD::equal(__x.__elems_, __x.__elems_ + _Size, __y.__elems_);
}

template <class _Tp, size_t _Size>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator!=(const array<_Tp, _Size>& __x, const array<_Tp, _Size>& __y)
{
    return !(__x == __y);
}

template <class _Tp, size_t _Size>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<(const array<_Tp, _Size>& __x, const array<_Tp, _Size>& __y)
{
    return _VSTD::lexicographical_compare(__x.__elems_, __x.__elems_ + _Size, __y.__elems_, __y.__elems_ + _Size);
}

template <class _Tp, size_t _Size>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>(const array<_Tp, _Size>& __x, const array<_Tp, _Size>& __y)
{
    return __y < __x;
}

template <class _Tp, size_t _Size>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<=(const array<_Tp, _Size>& __x, const array<_Tp, _Size>& __y)
{
    return !(__y < __x);
}

template <class _Tp, size_t _Size>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>=(const array<_Tp, _Size>& __x, const array<_Tp, _Size>& __y)
{
    return !(__x < __y);
}

template <class _Tp, size_t _Size>
inline _LIBCPP_INLINE_VISIBILITY
typename enable_if
<
    _Size == 0 ||
    __is_swappable<_Tp>::value,
    void
>::type
swap(array<_Tp, _Size>& __x, array<_Tp, _Size>& __y)
                                  _NOEXCEPT_(noexcept(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class _Tp, size_t _Size>
class _LIBCPP_TEMPLATE_VIS tuple_size<array<_Tp, _Size> >
    : public integral_constant<size_t, _Size> {};

template <size_t _Ip, class _Tp, size_t _Size>
class _LIBCPP_TEMPLATE_VIS tuple_element<_Ip, array<_Tp, _Size> >
{
    static_assert(_Ip < _Size, "Index out of bounds in std::tuple_element<> (std::array)");
public:
    typedef _Tp type;
};

template <size_t _Ip, class _Tp, size_t _Size>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
_Tp&
get(array<_Tp, _Size>& __a) _NOEXCEPT
{
    static_assert(_Ip < _Size, "Index out of bounds in std::get<> (std::array)");
    return __a.__elems_[_Ip];
}

template <size_t _Ip, class _Tp, size_t _Size>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
const _Tp&
get(const array<_Tp, _Size>& __a) _NOEXCEPT
{
    static_assert(_Ip < _Size, "Index out of bounds in std::get<> (const std::array)");
    return __a.__elems_[_Ip];
}

#ifndef _LIBCPP_CXX03_LANG

template <size_t _Ip, class _Tp, size_t _Size>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
_Tp&&
get(array<_Tp, _Size>&& __a) _NOEXCEPT
{
    static_assert(_Ip < _Size, "Index out of bounds in std::get<> (std::array &&)");
    return _VSTD::move(__a.__elems_[_Ip]);
}

template <size_t _Ip, class _Tp, size_t _Size>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
const _Tp&&
get(const array<_Tp, _Size>&& __a) _NOEXCEPT
{
    static_assert(_Ip < _Size, "Index out of bounds in std::get<> (const std::array &&)");
    return _VSTD::move(__a.__elems_[_Ip]);
}

#endif  // !_LIBCPP_CXX03_LANG

_LIBCPP_END_NAMESPACE_STD

#endif  // _LIBCPP_ARRAY
