// -*- C++ -*-
//===------------------------ functional ----------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_FUNCTIONAL
#define _LIBCPP_FUNCTIONAL

#include "__config.hpp"
#include "type_traits.hpp"
#include "typeinfo.hpp"
#include "memory.hpp"
#include "tuple.hpp"
#include "utility.hpp"

#include "__functional_base.hpp"

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#if _LIBCPP_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LIBCPP_TEMPLATE_VIS plus : binary_function<_Tp, _Tp, _Tp>
{
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x, const _Tp& __y) const
        {return __x + __y;}
};

#if _LIBCPP_STD_VER > 11
template <>
struct _LIBCPP_TEMPLATE_VIS plus<void>
{
    template <class _T1, class _T2>
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
    _NOEXCEPT_(noexcept(_VSTD::forward<_T1>(__t) + _VSTD::forward<_T2>(__u)))
    -> decltype        (_VSTD::forward<_T1>(__t) + _VSTD::forward<_T2>(__u))
        { return        _VSTD::forward<_T1>(__t) + _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif


#if _LIBCPP_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LIBCPP_TEMPLATE_VIS minus : binary_function<_Tp, _Tp, _Tp>
{
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x, const _Tp& __y) const
        {return __x - __y;}
};

#if _LIBCPP_STD_VER > 11
template <>
struct _LIBCPP_TEMPLATE_VIS minus<void>
{
    template <class _T1, class _T2>
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
    _NOEXCEPT_(noexcept(_VSTD::forward<_T1>(__t) - _VSTD::forward<_T2>(__u)))
    -> decltype        (_VSTD::forward<_T1>(__t) - _VSTD::forward<_T2>(__u))
        { return        _VSTD::forward<_T1>(__t) - _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif


#if _LIBCPP_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LIBCPP_TEMPLATE_VIS multiplies : binary_function<_Tp, _Tp, _Tp>
{
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x, const _Tp& __y) const
        {return __x * __y;}
};

#if _LIBCPP_STD_VER > 11
template <>
struct _LIBCPP_TEMPLATE_VIS multiplies<void>
{
    template <class _T1, class _T2>
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
    _NOEXCEPT_(noexcept(_VSTD::forward<_T1>(__t) * _VSTD::forward<_T2>(__u)))
    -> decltype        (_VSTD::forward<_T1>(__t) * _VSTD::forward<_T2>(__u))
        { return        _VSTD::forward<_T1>(__t) * _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif


#if _LIBCPP_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LIBCPP_TEMPLATE_VIS divides : binary_function<_Tp, _Tp, _Tp>
{
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x, const _Tp& __y) const
        {return __x / __y;}
};

#if _LIBCPP_STD_VER > 11
template <>
struct _LIBCPP_TEMPLATE_VIS divides<void>
{
    template <class _T1, class _T2>
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
    _NOEXCEPT_(noexcept(_VSTD::forward<_T1>(__t) / _VSTD::forward<_T2>(__u)))
    -> decltype        (_VSTD::forward<_T1>(__t) / _VSTD::forward<_T2>(__u))
        { return        _VSTD::forward<_T1>(__t) / _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif


#if _LIBCPP_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LIBCPP_TEMPLATE_VIS modulus : binary_function<_Tp, _Tp, _Tp>
{
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x, const _Tp& __y) const
        {return __x % __y;}
};

#if _LIBCPP_STD_VER > 11
template <>
struct _LIBCPP_TEMPLATE_VIS modulus<void>
{
    template <class _T1, class _T2>
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
    _NOEXCEPT_(noexcept(_VSTD::forward<_T1>(__t) % _VSTD::forward<_T2>(__u)))
    -> decltype        (_VSTD::forward<_T1>(__t) % _VSTD::forward<_T2>(__u))
        { return        _VSTD::forward<_T1>(__t) % _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif


#if _LIBCPP_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LIBCPP_TEMPLATE_VIS negate : unary_function<_Tp, _Tp>
{
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x) const
        {return -__x;}
};

#if _LIBCPP_STD_VER > 11
template <>
struct _LIBCPP_TEMPLATE_VIS negate<void>
{
    template <class _Tp>
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    auto operator()(_Tp&& __x) const
    _NOEXCEPT_(noexcept(- _VSTD::forward<_Tp>(__x)))
    -> decltype        (- _VSTD::forward<_Tp>(__x))
        { return        - _VSTD::forward<_Tp>(__x); }
    typedef void is_transparent;
};
#endif


#if _LIBCPP_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LIBCPP_TEMPLATE_VIS equal_to : binary_function<_Tp, _Tp, bool>
{
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    bool operator()(const _Tp& __x, const _Tp& __y) const
        {return __x == __y;}
};

#if _LIBCPP_STD_VER > 11
template <>
struct _LIBCPP_TEMPLATE_VIS equal_to<void>
{
    template <class _T1, class _T2>
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
    _NOEXCEPT_(noexcept(_VSTD::forward<_T1>(__t) == _VSTD::forward<_T2>(__u)))
    -> decltype        (_VSTD::forward<_T1>(__t) == _VSTD::forward<_T2>(__u))
        { return        _VSTD::forward<_T1>(__t) == _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif


#if _LIBCPP_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LIBCPP_TEMPLATE_VIS not_equal_to : binary_function<_Tp, _Tp, bool>
{
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    bool operator()(const _Tp& __x, const _Tp& __y) const
        {return __x != __y;}
};

#if _LIBCPP_STD_VER > 11
template <>
struct _LIBCPP_TEMPLATE_VIS not_equal_to<void>
{
    template <class _T1, class _T2>
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
    _NOEXCEPT_(noexcept(_VSTD::forward<_T1>(__t) != _VSTD::forward<_T2>(__u)))
    -> decltype        (_VSTD::forward<_T1>(__t) != _VSTD::forward<_T2>(__u))
        { return        _VSTD::forward<_T1>(__t) != _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif


#if _LIBCPP_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LIBCPP_TEMPLATE_VIS greater : binary_function<_Tp, _Tp, bool>
{
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    bool operator()(const _Tp& __x, const _Tp& __y) const
        {return __x > __y;}
};

#if _LIBCPP_STD_VER > 11
template <>
struct _LIBCPP_TEMPLATE_VIS greater<void>
{
    template <class _T1, class _T2>
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
    _NOEXCEPT_(noexcept(_VSTD::forward<_T1>(__t) > _VSTD::forward<_T2>(__u)))
    -> decltype        (_VSTD::forward<_T1>(__t) > _VSTD::forward<_T2>(__u))
        { return        _VSTD::forward<_T1>(__t) > _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif


// less in <__functional_base>

#if _LIBCPP_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LIBCPP_TEMPLATE_VIS greater_equal : binary_function<_Tp, _Tp, bool>
{
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    bool operator()(const _Tp& __x, const _Tp& __y) const
        {return __x >= __y;}
};

#if _LIBCPP_STD_VER > 11
template <>
struct _LIBCPP_TEMPLATE_VIS greater_equal<void>
{
    template <class _T1, class _T2>
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
    _NOEXCEPT_(noexcept(_VSTD::forward<_T1>(__t) >= _VSTD::forward<_T2>(__u)))
    -> decltype        (_VSTD::forward<_T1>(__t) >= _VSTD::forward<_T2>(__u))
        { return        _VSTD::forward<_T1>(__t) >= _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif


#if _LIBCPP_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LIBCPP_TEMPLATE_VIS less_equal : binary_function<_Tp, _Tp, bool>
{
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    bool operator()(const _Tp& __x, const _Tp& __y) const
        {return __x <= __y;}
};

#if _LIBCPP_STD_VER > 11
template <>
struct _LIBCPP_TEMPLATE_VIS less_equal<void>
{
    template <class _T1, class _T2>
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
    _NOEXCEPT_(noexcept(_VSTD::forward<_T1>(__t) <= _VSTD::forward<_T2>(__u)))
    -> decltype        (_VSTD::forward<_T1>(__t) <= _VSTD::forward<_T2>(__u))
        { return        _VSTD::forward<_T1>(__t) <= _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif


#if _LIBCPP_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LIBCPP_TEMPLATE_VIS logical_and : binary_function<_Tp, _Tp, bool>
{
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    bool operator()(const _Tp& __x, const _Tp& __y) const
        {return __x && __y;}
};

#if _LIBCPP_STD_VER > 11
template <>
struct _LIBCPP_TEMPLATE_VIS logical_and<void>
{
    template <class _T1, class _T2>
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
    _NOEXCEPT_(noexcept(_VSTD::forward<_T1>(__t) && _VSTD::forward<_T2>(__u)))
    -> decltype        (_VSTD::forward<_T1>(__t) && _VSTD::forward<_T2>(__u))
        { return        _VSTD::forward<_T1>(__t) && _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif


#if _LIBCPP_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LIBCPP_TEMPLATE_VIS logical_or : binary_function<_Tp, _Tp, bool>
{
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    bool operator()(const _Tp& __x, const _Tp& __y) const
        {return __x || __y;}
};

#if _LIBCPP_STD_VER > 11
template <>
struct _LIBCPP_TEMPLATE_VIS logical_or<void>
{
    template <class _T1, class _T2>
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
    _NOEXCEPT_(noexcept(_VSTD::forward<_T1>(__t) || _VSTD::forward<_T2>(__u)))
    -> decltype        (_VSTD::forward<_T1>(__t) || _VSTD::forward<_T2>(__u))
        { return        _VSTD::forward<_T1>(__t) || _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif


#if _LIBCPP_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LIBCPP_TEMPLATE_VIS logical_not : unary_function<_Tp, bool>
{
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    bool operator()(const _Tp& __x) const
        {return !__x;}
};

#if _LIBCPP_STD_VER > 11
template <>
struct _LIBCPP_TEMPLATE_VIS logical_not<void>
{
    template <class _Tp>
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    auto operator()(_Tp&& __x) const
    _NOEXCEPT_(noexcept(!_VSTD::forward<_Tp>(__x)))
    -> decltype        (!_VSTD::forward<_Tp>(__x))
        { return        !_VSTD::forward<_Tp>(__x); }
    typedef void is_transparent;
};
#endif


#if _LIBCPP_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LIBCPP_TEMPLATE_VIS bit_and : binary_function<_Tp, _Tp, _Tp>
{
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x, const _Tp& __y) const
        {return __x & __y;}
};

#if _LIBCPP_STD_VER > 11
template <>
struct _LIBCPP_TEMPLATE_VIS bit_and<void>
{
    template <class _T1, class _T2>
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
    _NOEXCEPT_(noexcept(_VSTD::forward<_T1>(__t) & _VSTD::forward<_T2>(__u)))
    -> decltype        (_VSTD::forward<_T1>(__t) & _VSTD::forward<_T2>(__u))
        { return        _VSTD::forward<_T1>(__t) & _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif


#if _LIBCPP_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LIBCPP_TEMPLATE_VIS bit_or : binary_function<_Tp, _Tp, _Tp>
{
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x, const _Tp& __y) const
        {return __x | __y;}
};

#if _LIBCPP_STD_VER > 11
template <>
struct _LIBCPP_TEMPLATE_VIS bit_or<void>
{
    template <class _T1, class _T2>
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
    _NOEXCEPT_(noexcept(_VSTD::forward<_T1>(__t) | _VSTD::forward<_T2>(__u)))
    -> decltype        (_VSTD::forward<_T1>(__t) | _VSTD::forward<_T2>(__u))
        { return        _VSTD::forward<_T1>(__t) | _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif


#if _LIBCPP_STD_VER > 11
template <class _Tp = void>
#else
template <class _Tp>
#endif
struct _LIBCPP_TEMPLATE_VIS bit_xor : binary_function<_Tp, _Tp, _Tp>
{
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x, const _Tp& __y) const
        {return __x ^ __y;}
};

#if _LIBCPP_STD_VER > 11
template <>
struct _LIBCPP_TEMPLATE_VIS bit_xor<void>
{
    template <class _T1, class _T2>
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    auto operator()(_T1&& __t, _T2&& __u) const
    _NOEXCEPT_(noexcept(_VSTD::forward<_T1>(__t) ^ _VSTD::forward<_T2>(__u)))
    -> decltype        (_VSTD::forward<_T1>(__t) ^ _VSTD::forward<_T2>(__u))
        { return        _VSTD::forward<_T1>(__t) ^ _VSTD::forward<_T2>(__u); }
    typedef void is_transparent;
};
#endif


#if _LIBCPP_STD_VER > 11
template <class _Tp = void>
struct _LIBCPP_TEMPLATE_VIS bit_not : unary_function<_Tp, _Tp>
{
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    _Tp operator()(const _Tp& __x) const
        {return ~__x;}
};

template <>
struct _LIBCPP_TEMPLATE_VIS bit_not<void>
{
    template <class _Tp>
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    auto operator()(_Tp&& __x) const
    _NOEXCEPT_(noexcept(~_VSTD::forward<_Tp>(__x)))
    -> decltype        (~_VSTD::forward<_Tp>(__x))
        { return        ~_VSTD::forward<_Tp>(__x); }
    typedef void is_transparent;
};
#endif

template <class _Predicate>
class _LIBCPP_TEMPLATE_VIS unary_negate
    : public unary_function<typename _Predicate::argument_type, bool>
{
    _Predicate __pred_;
public:
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    explicit unary_negate(const _Predicate& __pred)
        : __pred_(__pred) {}
    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    bool operator()(const typename _Predicate::argument_type& __x) const
        {return !__pred_(__x);}
};

template <class _Predicate>
inline _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
unary_negate<_Predicate>
not1(const _Predicate& __pred) {return unary_negate<_Predicate>(__pred);}

template <class _Predicate>
class _LIBCPP_TEMPLATE_VIS binary_negate
    : public binary_function<typename _Predicate::first_argument_type,
                             typename _Predicate::second_argument_type,
                             bool>
{
    _Predicate __pred_;
public:
    _LIBCPP_INLINE_VISIBILITY explicit _LIBCPP_CONSTEXPR_AFTER_CXX11
    binary_negate(const _Predicate& __pred) : __pred_(__pred) {}

    _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
    bool operator()(const typename _Predicate::first_argument_type& __x,
                    const typename _Predicate::second_argument_type& __y) const
        {return !__pred_(__x, __y);}
};

template <class _Predicate>
inline _LIBCPP_CONSTEXPR_AFTER_CXX11 _LIBCPP_INLINE_VISIBILITY
binary_negate<_Predicate>
not2(const _Predicate& __pred) {return binary_negate<_Predicate>(__pred);}

#if _LIBCPP_STD_VER <= 14 || defined(_LIBCPP_ENABLE_CXX17_REMOVED_BINDERS)
template <class __Operation>
class _LIBCPP_TEMPLATE_VIS binder1st
    : public unary_function<typename __Operation::second_argument_type,
                            typename __Operation::result_type>
{
protected:
    __Operation                               op;
    typename __Operation::first_argument_type value;
public:
    _LIBCPP_INLINE_VISIBILITY binder1st(const __Operation& __x,
                               const typename __Operation::first_argument_type __y)
        : op(__x), value(__y) {}
    _LIBCPP_INLINE_VISIBILITY typename __Operation::result_type operator()
        (typename __Operation::second_argument_type& __x) const
            {return op(value, __x);}
    _LIBCPP_INLINE_VISIBILITY typename __Operation::result_type operator()
        (const typename __Operation::second_argument_type& __x) const
            {return op(value, __x);}
};

template <class __Operation, class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
binder1st<__Operation>
bind1st(const __Operation& __op, const _Tp& __x)
    {return binder1st<__Operation>(__op, __x);}

template <class __Operation>
class _LIBCPP_TEMPLATE_VIS binder2nd
    : public unary_function<typename __Operation::first_argument_type,
                            typename __Operation::result_type>
{
protected:
    __Operation                                op;
    typename __Operation::second_argument_type value;
public:
    _LIBCPP_INLINE_VISIBILITY
    binder2nd(const __Operation& __x, const typename __Operation::second_argument_type __y)
        : op(__x), value(__y) {}
    _LIBCPP_INLINE_VISIBILITY typename __Operation::result_type operator()
        (      typename __Operation::first_argument_type& __x) const
            {return op(__x, value);}
    _LIBCPP_INLINE_VISIBILITY typename __Operation::result_type operator()
        (const typename __Operation::first_argument_type& __x) const
            {return op(__x, value);}
};

template <class __Operation, class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
binder2nd<__Operation>
bind2nd(const __Operation& __op, const _Tp& __x)
    {return binder2nd<__Operation>(__op, __x);}

template <class _Arg, class _Result>
class _LIBCPP_TEMPLATE_VIS pointer_to_unary_function
    : public unary_function<_Arg, _Result>
{
    _Result (*__f_)(_Arg);
public:
    _LIBCPP_INLINE_VISIBILITY explicit pointer_to_unary_function(_Result (*__f)(_Arg))
        : __f_(__f) {}
    _LIBCPP_INLINE_VISIBILITY _Result operator()(_Arg __x) const
        {return __f_(__x);}
};

template <class _Arg, class _Result>
inline _LIBCPP_INLINE_VISIBILITY
pointer_to_unary_function<_Arg,_Result>
ptr_fun(_Result (*__f)(_Arg))
    {return pointer_to_unary_function<_Arg,_Result>(__f);}

template <class _Arg1, class _Arg2, class _Result>
class _LIBCPP_TEMPLATE_VIS pointer_to_binary_function
    : public binary_function<_Arg1, _Arg2, _Result>
{
    _Result (*__f_)(_Arg1, _Arg2);
public:
    _LIBCPP_INLINE_VISIBILITY explicit pointer_to_binary_function(_Result (*__f)(_Arg1, _Arg2))
        : __f_(__f) {}
    _LIBCPP_INLINE_VISIBILITY _Result operator()(_Arg1 __x, _Arg2 __y) const
        {return __f_(__x, __y);}
};

template <class _Arg1, class _Arg2, class _Result>
inline _LIBCPP_INLINE_VISIBILITY
pointer_to_binary_function<_Arg1,_Arg2,_Result>
ptr_fun(_Result (*__f)(_Arg1,_Arg2))
    {return pointer_to_binary_function<_Arg1,_Arg2,_Result>(__f);}

template<class _Sp, class _Tp>
class _LIBCPP_TEMPLATE_VIS mem_fun_t : public unary_function<_Tp*, _Sp>
{
    _Sp (_Tp::*__p_)();
public:
    _LIBCPP_INLINE_VISIBILITY explicit mem_fun_t(_Sp (_Tp::*__p)())
        : __p_(__p) {}
    _LIBCPP_INLINE_VISIBILITY _Sp operator()(_Tp* __p) const
        {return (__p->*__p_)();}
};

template<class _Sp, class _Tp, class _Ap>
class _LIBCPP_TEMPLATE_VIS mem_fun1_t : public binary_function<_Tp*, _Ap, _Sp>
{
    _Sp (_Tp::*__p_)(_Ap);
public:
    _LIBCPP_INLINE_VISIBILITY explicit mem_fun1_t(_Sp (_Tp::*__p)(_Ap))
        : __p_(__p) {}
    _LIBCPP_INLINE_VISIBILITY _Sp operator()(_Tp* __p, _Ap __x) const
        {return (__p->*__p_)(__x);}
};

template<class _Sp, class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
mem_fun_t<_Sp,_Tp>
mem_fun(_Sp (_Tp::*__f)())
    {return mem_fun_t<_Sp,_Tp>(__f);}

template<class _Sp, class _Tp, class _Ap>
inline _LIBCPP_INLINE_VISIBILITY
mem_fun1_t<_Sp,_Tp,_Ap>
mem_fun(_Sp (_Tp::*__f)(_Ap))
    {return mem_fun1_t<_Sp,_Tp,_Ap>(__f);}

template<class _Sp, class _Tp>
class _LIBCPP_TEMPLATE_VIS mem_fun_ref_t : public unary_function<_Tp, _Sp>
{
    _Sp (_Tp::*__p_)();
public:
    _LIBCPP_INLINE_VISIBILITY explicit mem_fun_ref_t(_Sp (_Tp::*__p)())
        : __p_(__p) {}
    _LIBCPP_INLINE_VISIBILITY _Sp operator()(_Tp& __p) const
        {return (__p.*__p_)();}
};

template<class _Sp, class _Tp, class _Ap>
class _LIBCPP_TEMPLATE_VIS mem_fun1_ref_t : public binary_function<_Tp, _Ap, _Sp>
{
    _Sp (_Tp::*__p_)(_Ap);
public:
    _LIBCPP_INLINE_VISIBILITY explicit mem_fun1_ref_t(_Sp (_Tp::*__p)(_Ap))
        : __p_(__p) {}
    _LIBCPP_INLINE_VISIBILITY _Sp operator()(_Tp& __p, _Ap __x) const
        {return (__p.*__p_)(__x);}
};

template<class _Sp, class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
mem_fun_ref_t<_Sp,_Tp>
mem_fun_ref(_Sp (_Tp::*__f)())
    {return mem_fun_ref_t<_Sp,_Tp>(__f);}

template<class _Sp, class _Tp, class _Ap>
inline _LIBCPP_INLINE_VISIBILITY
mem_fun1_ref_t<_Sp,_Tp,_Ap>
mem_fun_ref(_Sp (_Tp::*__f)(_Ap))
    {return mem_fun1_ref_t<_Sp,_Tp,_Ap>(__f);}

template <class _Sp, class _Tp>
class _LIBCPP_TEMPLATE_VIS const_mem_fun_t : public unary_function<const _Tp*, _Sp>
{
    _Sp (_Tp::*__p_)() const;
public:
    _LIBCPP_INLINE_VISIBILITY explicit const_mem_fun_t(_Sp (_Tp::*__p)() const)
        : __p_(__p) {}
    _LIBCPP_INLINE_VISIBILITY _Sp operator()(const _Tp* __p) const
        {return (__p->*__p_)();}
};

template <class _Sp, class _Tp, class _Ap>
class _LIBCPP_TEMPLATE_VIS const_mem_fun1_t : public binary_function<const _Tp*, _Ap, _Sp>
{
    _Sp (_Tp::*__p_)(_Ap) const;
public:
    _LIBCPP_INLINE_VISIBILITY explicit const_mem_fun1_t(_Sp (_Tp::*__p)(_Ap) const)
        : __p_(__p) {}
    _LIBCPP_INLINE_VISIBILITY _Sp operator()(const _Tp* __p, _Ap __x) const
        {return (__p->*__p_)(__x);}
};

template <class _Sp, class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
const_mem_fun_t<_Sp,_Tp>
mem_fun(_Sp (_Tp::*__f)() const)
    {return const_mem_fun_t<_Sp,_Tp>(__f);}

template <class _Sp, class _Tp, class _Ap>
inline _LIBCPP_INLINE_VISIBILITY
const_mem_fun1_t<_Sp,_Tp,_Ap>
mem_fun(_Sp (_Tp::*__f)(_Ap) const)
    {return const_mem_fun1_t<_Sp,_Tp,_Ap>(__f);}

template <class _Sp, class _Tp>
class _LIBCPP_TEMPLATE_VIS const_mem_fun_ref_t : public unary_function<_Tp, _Sp>
{
    _Sp (_Tp::*__p_)() const;
public:
    _LIBCPP_INLINE_VISIBILITY explicit const_mem_fun_ref_t(_Sp (_Tp::*__p)() const)
        : __p_(__p) {}
    _LIBCPP_INLINE_VISIBILITY _Sp operator()(const _Tp& __p) const
        {return (__p.*__p_)();}
};

template <class _Sp, class _Tp, class _Ap>
class _LIBCPP_TEMPLATE_VIS const_mem_fun1_ref_t
    : public binary_function<_Tp, _Ap, _Sp>
{
    _Sp (_Tp::*__p_)(_Ap) const;
public:
    _LIBCPP_INLINE_VISIBILITY explicit const_mem_fun1_ref_t(_Sp (_Tp::*__p)(_Ap) const)
        : __p_(__p) {}
    _LIBCPP_INLINE_VISIBILITY _Sp operator()(const _Tp& __p, _Ap __x) const
        {return (__p.*__p_)(__x);}
};

template <class _Sp, class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
const_mem_fun_ref_t<_Sp,_Tp>
mem_fun_ref(_Sp (_Tp::*__f)() const)
    {return const_mem_fun_ref_t<_Sp,_Tp>(__f);}

template <class _Sp, class _Tp, class _Ap>
inline _LIBCPP_INLINE_VISIBILITY
const_mem_fun1_ref_t<_Sp,_Tp,_Ap>
mem_fun_ref(_Sp (_Tp::*__f)(_Ap) const)
    {return const_mem_fun1_ref_t<_Sp,_Tp,_Ap>(__f);}
#endif

////////////////////////////////////////////////////////////////////////////////
//                                MEMFUN
//==============================================================================

template <class _Tp>
class __mem_fn
    : public __weak_result_type<_Tp>
{
public:
    // types
    typedef _Tp type;
private:
    type __f_;

public:
    _LIBCPP_INLINE_VISIBILITY __mem_fn(type __f) _NOEXCEPT : __f_(__f) {}

#ifndef _LIBCPP_CXX03_LANG
    // invoke
    template <class... _ArgTypes>
    _LIBCPP_INLINE_VISIBILITY
    typename __invoke_return<type, _ArgTypes...>::type
    operator() (_ArgTypes&&... __args) const {
        return __invoke(__f_, _VSTD::forward<_ArgTypes>(__args)...);
    }
#else

    template <class _A0>
    _LIBCPP_INLINE_VISIBILITY
    typename __invoke_return0<type, _A0>::type
    operator() (_A0& __a0) const {
        return __invoke(__f_, __a0);
    }

    template <class _A0>
    _LIBCPP_INLINE_VISIBILITY
    typename __invoke_return0<type, _A0 const>::type
    operator() (_A0 const& __a0) const {
        return __invoke(__f_, __a0);
    }

    template <class _A0, class _A1>
    _LIBCPP_INLINE_VISIBILITY
    typename __invoke_return1<type, _A0, _A1>::type
    operator() (_A0& __a0, _A1& __a1) const {
        return __invoke(__f_, __a0, __a1);
    }

    template <class _A0, class _A1>
    _LIBCPP_INLINE_VISIBILITY
    typename __invoke_return1<type, _A0 const, _A1>::type
    operator() (_A0 const& __a0, _A1& __a1) const {
        return __invoke(__f_, __a0, __a1);
    }

    template <class _A0, class _A1>
    _LIBCPP_INLINE_VISIBILITY
    typename __invoke_return1<type, _A0, _A1 const>::type
    operator() (_A0& __a0, _A1 const& __a1) const {
        return __invoke(__f_, __a0, __a1);
    }

    template <class _A0, class _A1>
    _LIBCPP_INLINE_VISIBILITY
    typename __invoke_return1<type, _A0 const, _A1 const>::type
    operator() (_A0 const& __a0, _A1 const& __a1) const {
        return __invoke(__f_, __a0, __a1);
    }

    template <class _A0, class _A1, class _A2>
    _LIBCPP_INLINE_VISIBILITY
    typename __invoke_return2<type, _A0, _A1, _A2>::type
    operator() (_A0& __a0, _A1& __a1, _A2& __a2) const {
        return __invoke(__f_, __a0, __a1, __a2);
    }

    template <class _A0, class _A1, class _A2>
    _LIBCPP_INLINE_VISIBILITY
    typename __invoke_return2<type, _A0 const, _A1, _A2>::type
    operator() (_A0 const& __a0, _A1& __a1, _A2& __a2) const {
        return __invoke(__f_, __a0, __a1, __a2);
    }

    template <class _A0, class _A1, class _A2>
    _LIBCPP_INLINE_VISIBILITY
    typename __invoke_return2<type, _A0, _A1 const, _A2>::type
    operator() (_A0& __a0, _A1 const& __a1, _A2& __a2) const {
        return __invoke(__f_, __a0, __a1, __a2);
    }

    template <class _A0, class _A1, class _A2>
    _LIBCPP_INLINE_VISIBILITY
    typename __invoke_return2<type, _A0, _A1, _A2 const>::type
    operator() (_A0& __a0, _A1& __a1, _A2 const& __a2) const {
        return __invoke(__f_, __a0, __a1, __a2);
    }

    template <class _A0, class _A1, class _A2>
    _LIBCPP_INLINE_VISIBILITY
    typename __invoke_return2<type, _A0 const, _A1 const, _A2>::type
    operator() (_A0 const& __a0, _A1 const& __a1, _A2& __a2) const {
        return __invoke(__f_, __a0, __a1, __a2);
    }

    template <class _A0, class _A1, class _A2>
    _LIBCPP_INLINE_VISIBILITY
    typename __invoke_return2<type, _A0 const, _A1, _A2 const>::type
    operator() (_A0 const& __a0, _A1& __a1, _A2 const& __a2) const {
        return __invoke(__f_, __a0, __a1, __a2);
    }

    template <class _A0, class _A1, class _A2>
    _LIBCPP_INLINE_VISIBILITY
    typename __invoke_return2<type, _A0, _A1 const, _A2 const>::type
    operator() (_A0& __a0, _A1 const& __a1, _A2 const& __a2) const {
        return __invoke(__f_, __a0, __a1, __a2);
    }

    template <class _A0, class _A1, class _A2>
    _LIBCPP_INLINE_VISIBILITY
    typename __invoke_return2<type, _A0 const, _A1 const, _A2 const>::type
    operator() (_A0 const& __a0, _A1 const& __a1, _A2 const& __a2) const {
        return __invoke(__f_, __a0, __a1, __a2);
    }
#endif
};

template<class _Rp, class _Tp>
inline _LIBCPP_INLINE_VISIBILITY
__mem_fn<_Rp _Tp::*>
mem_fn(_Rp _Tp::* __pm) _NOEXCEPT
{
    return __mem_fn<_Rp _Tp::*>(__pm);
}

////////////////////////////////////////////////////////////////////////////////
//                                FUNCTION
//==============================================================================


_LIBCPP_NORETURN inline _LIBCPP_ALWAYS_INLINE
void __throw_bad_function_call()
{
#ifndef _LIBCPP_NO_EXCEPTIONS
    throw bad_function_call();
#else
    abort();
#endif
}

template<class _Fp> class _LIBCPP_TEMPLATE_VIS function; // undefined

namespace __function
{

template<class _Rp>
struct __maybe_derive_from_unary_function
{
};

template<class _Rp, class _A1>
struct __maybe_derive_from_unary_function<_Rp(_A1)>
    : public unary_function<_A1, _Rp>
{
};

template<class _Rp>
struct __maybe_derive_from_binary_function
{
};

template<class _Rp, class _A1, class _A2>
struct __maybe_derive_from_binary_function<_Rp(_A1, _A2)>
    : public binary_function<_A1, _A2, _Rp>
{
};

template <class _Fp>
_LIBCPP_INLINE_VISIBILITY
bool __not_null(_Fp const&) { return true; }

template <class _Fp>
_LIBCPP_INLINE_VISIBILITY
bool __not_null(_Fp* __ptr) { return __ptr; }

template <class _Ret, class _Class>
_LIBCPP_INLINE_VISIBILITY
bool __not_null(_Ret _Class::*__ptr) { return __ptr; }

template <class _Fp>
_LIBCPP_INLINE_VISIBILITY
bool __not_null(function<_Fp> const& __f) { return !!__f; }

} // namespace __function

#ifndef _LIBCPP_CXX03_LANG

namespace __function {

template<class _Fp> class __base;

template<class _Rp, class ..._ArgTypes>
class __base<_Rp(_ArgTypes...)>
{
    __base(const __base&);
    __base& operator=(const __base&);
public:
    _LIBCPP_INLINE_VISIBILITY __base() {}
    _LIBCPP_INLINE_VISIBILITY virtual ~__base() {}
    virtual __base* __clone() const = 0;
    virtual void __clone(__base*) const = 0;
    virtual void destroy() _NOEXCEPT = 0;
    virtual void destroy_deallocate() _NOEXCEPT = 0;
    virtual _Rp operator()(_ArgTypes&& ...) = 0;
#ifndef _LIBCPP_NO_RTTI
    virtual const void* target(const type_info&) const _NOEXCEPT = 0;
    virtual const std::type_info& target_type() const _NOEXCEPT = 0;
#endif  // _LIBCPP_NO_RTTI
};

template<class _FD, class _Alloc, class _FB> class __func;

template<class _Fp, class _Alloc, class _Rp, class ..._ArgTypes>
class __func<_Fp, _Alloc, _Rp(_ArgTypes...)>
    : public  __base<_Rp(_ArgTypes...)>
{
    __compressed_pair<_Fp, _Alloc> __f_;
public:
    _LIBCPP_INLINE_VISIBILITY
    explicit __func(_Fp&& __f)
        : __f_(piecewise_construct, _VSTD::forward_as_tuple(_VSTD::move(__f)),
                                    _VSTD::forward_as_tuple()) {}
    _LIBCPP_INLINE_VISIBILITY
    explicit __func(const _Fp& __f, const _Alloc& __a)
        : __f_(piecewise_construct, _VSTD::forward_as_tuple(__f),
                                    _VSTD::forward_as_tuple(__a)) {}

    _LIBCPP_INLINE_VISIBILITY
    explicit __func(const _Fp& __f, _Alloc&& __a)
        : __f_(piecewise_construct, _VSTD::forward_as_tuple(__f),
                                    _VSTD::forward_as_tuple(_VSTD::move(__a))) {}

    _LIBCPP_INLINE_VISIBILITY
    explicit __func(_Fp&& __f, _Alloc&& __a)
        : __f_(piecewise_construct, _VSTD::forward_as_tuple(_VSTD::move(__f)),
                                    _VSTD::forward_as_tuple(_VSTD::move(__a))) {}
    virtual __base<_Rp(_ArgTypes...)>* __clone() const;
    virtual void __clone(__base<_Rp(_ArgTypes...)>*) const;
    virtual void destroy() _NOEXCEPT;
    virtual void destroy_deallocate() _NOEXCEPT;
    virtual _Rp operator()(_ArgTypes&& ... __arg);
#ifndef _LIBCPP_NO_RTTI
    virtual const void* target(const type_info&) const _NOEXCEPT;
    virtual const std::type_info& target_type() const _NOEXCEPT;
#endif  // _LIBCPP_NO_RTTI
};

template<class _Fp, class _Alloc, class _Rp, class ..._ArgTypes>
__base<_Rp(_ArgTypes...)>*
__func<_Fp, _Alloc, _Rp(_ArgTypes...)>::__clone() const
{
    typedef allocator_traits<_Alloc> __alloc_traits;
    typedef typename __rebind_alloc_helper<__alloc_traits, __func>::type _Ap;
    _Ap __a(__f_.second());
    typedef __allocator_destructor<_Ap> _Dp;
    unique_ptr<__func, _Dp> __hold(__a.allocate(1), _Dp(__a, 1));
    ::new (__hold.get()) __func(__f_.first(), _Alloc(__a));
    return __hold.release();
}

template<class _Fp, class _Alloc, class _Rp, class ..._ArgTypes>
void
__func<_Fp, _Alloc, _Rp(_ArgTypes...)>::__clone(__base<_Rp(_ArgTypes...)>* __p) const
{
    ::new (__p) __func(__f_.first(), __f_.second());
}

template<class _Fp, class _Alloc, class _Rp, class ..._ArgTypes>
void
__func<_Fp, _Alloc, _Rp(_ArgTypes...)>::destroy() _NOEXCEPT
{
    __f_.~__compressed_pair<_Fp, _Alloc>();
}

template<class _Fp, class _Alloc, class _Rp, class ..._ArgTypes>
void
__func<_Fp, _Alloc, _Rp(_ArgTypes...)>::destroy_deallocate() _NOEXCEPT
{
    typedef allocator_traits<_Alloc> __alloc_traits;
    typedef typename __rebind_alloc_helper<__alloc_traits, __func>::type _Ap;
    _Ap __a(__f_.second());
    __f_.~__compressed_pair<_Fp, _Alloc>();
    __a.deallocate(this, 1);
}

template<class _Fp, class _Alloc, class _Rp, class ..._ArgTypes>
_Rp
__func<_Fp, _Alloc, _Rp(_ArgTypes...)>::operator()(_ArgTypes&& ... __arg)
{
    typedef __invoke_void_return_wrapper<_Rp> _Invoker;
    return _Invoker::__call(__f_.first(), _VSTD::forward<_ArgTypes>(__arg)...);
}

#ifndef _LIBCPP_NO_RTTI

template<class _Fp, class _Alloc, class _Rp, class ..._ArgTypes>
const void*
__func<_Fp, _Alloc, _Rp(_ArgTypes...)>::target(const type_info& __ti) const _NOEXCEPT
{
    if (__ti == typeid(_Fp))
        return &__f_.first();
    return (const void*)0;
}

template<class _Fp, class _Alloc, class _Rp, class ..._ArgTypes>
const std::type_info&
__func<_Fp, _Alloc, _Rp(_ArgTypes...)>::target_type() const _NOEXCEPT
{
    return typeid(_Fp);
}

#endif  // _LIBCPP_NO_RTTI

}  // __function

template<class _Rp, class ..._ArgTypes>
class _LIBCPP_TEMPLATE_VIS function<_Rp(_ArgTypes...)>
    : public __function::__maybe_derive_from_unary_function<_Rp(_ArgTypes...)>,
      public __function::__maybe_derive_from_binary_function<_Rp(_ArgTypes...)>
{
    typedef __function::__base<_Rp(_ArgTypes...)> __base;
    typename aligned_storage<3*sizeof(void*)>::type __buf_;
    __base* __f_;

    _LIBCPP_NO_CFI static __base *__as_base(void *p) {
      return reinterpret_cast<__base*>(p);
    }

    template <class _Fp, bool = __lazy_and<
        integral_constant<bool, !is_same<__uncvref_t<_Fp>, function>::value>,
        __invokable<_Fp&, _ArgTypes...>
    >::value>
    struct __callable;
    template <class _Fp>
        struct __callable<_Fp, true>
        {
            static const bool value = is_same<void, _Rp>::value ||
                is_convertible<typename __invoke_of<_Fp&, _ArgTypes...>::type,
                               _Rp>::value;
        };
    template <class _Fp>
        struct __callable<_Fp, false>
        {
            static const bool value = false;
        };

  template <class _Fp>
  using _EnableIfCallable = typename enable_if<__callable<_Fp>::value>::type;
public:
    typedef _Rp result_type;

    // construct/copy/destroy:
    _LIBCPP_INLINE_VISIBILITY
    function() _NOEXCEPT : __f_(0) {}
    _LIBCPP_INLINE_VISIBILITY
    function(nullptr_t) _NOEXCEPT : __f_(0) {}
    function(const function&);
    function(function&&) _NOEXCEPT;
    template<class _Fp, class = _EnableIfCallable<_Fp>>
    function(_Fp);

#if _LIBCPP_STD_VER <= 14
    template<class _Alloc>
      _LIBCPP_INLINE_VISIBILITY
      function(allocator_arg_t, const _Alloc&) _NOEXCEPT : __f_(0) {}
    template<class _Alloc>
      _LIBCPP_INLINE_VISIBILITY
      function(allocator_arg_t, const _Alloc&, nullptr_t) _NOEXCEPT : __f_(0) {}
    template<class _Alloc>
      function(allocator_arg_t, const _Alloc&, const function&);
    template<class _Alloc>
      function(allocator_arg_t, const _Alloc&, function&&);
    template<class _Fp, class _Alloc, class = _EnableIfCallable<_Fp>>
      function(allocator_arg_t, const _Alloc& __a, _Fp __f);
#endif

    function& operator=(const function&);
    function& operator=(function&&) _NOEXCEPT;
    function& operator=(nullptr_t) _NOEXCEPT;
    template<class _Fp, class = _EnableIfCallable<_Fp>>
    function& operator=(_Fp&&);

    ~function();

    // function modifiers:
    void swap(function&) _NOEXCEPT;

#if _LIBCPP_STD_VER <= 14
    template<class _Fp, class _Alloc>
      _LIBCPP_INLINE_VISIBILITY
      void assign(_Fp&& __f, const _Alloc& __a)
        {function(allocator_arg, __a, _VSTD::forward<_Fp>(__f)).swap(*this);}
#endif

    // function capacity:
    inline
        _LIBCPP_EXPLICIT operator bool() const _NOEXCEPT {return __f_;}

    // deleted overloads close possible hole in the type system
    template<class _R2, class... _ArgTypes2>
      bool operator==(const function<_R2(_ArgTypes2...)>&) const = delete;
    template<class _R2, class... _ArgTypes2>
      bool operator!=(const function<_R2(_ArgTypes2...)>&) const = delete;
public:
    // function invocation:
    _Rp operator()(_ArgTypes...) const;

#ifndef _LIBCPP_NO_RTTI
    // function target access:
    const std::type_info& target_type() const _NOEXCEPT;
    template <typename _Tp> _Tp* target() _NOEXCEPT;
    template <typename _Tp> const _Tp* target() const _NOEXCEPT;
#endif  // _LIBCPP_NO_RTTI
};

template<class _Rp, class ..._ArgTypes>
function<_Rp(_ArgTypes...)>::function(const function& __f)
{
    if (__f.__f_ == 0)
        __f_ = 0;
    else if ((void *)__f.__f_ == &__f.__buf_)
    {
        __f_ = __as_base(&__buf_);
        __f.__f_->__clone(__f_);
    }
    else
        __f_ = __f.__f_->__clone();
}

#if _LIBCPP_STD_VER <= 14
template<class _Rp, class ..._ArgTypes>
template <class _Alloc>
function<_Rp(_ArgTypes...)>::function(allocator_arg_t, const _Alloc&,
                                     const function& __f)
{
    if (__f.__f_ == 0)
        __f_ = 0;
    else if ((void *)__f.__f_ == &__f.__buf_)
    {
        __f_ = __as_base(&__buf_);
        __f.__f_->__clone(__f_);
    }
    else
        __f_ = __f.__f_->__clone();
}
#endif

template<class _Rp, class ..._ArgTypes>
function<_Rp(_ArgTypes...)>::function(function&& __f) _NOEXCEPT
{
    if (__f.__f_ == 0)
        __f_ = 0;
    else if ((void *)__f.__f_ == &__f.__buf_)
    {
        __f_ = __as_base(&__buf_);
        __f.__f_->__clone(__f_);
    }
    else
    {
        __f_ = __f.__f_;
        __f.__f_ = 0;
    }
}

#if _LIBCPP_STD_VER <= 14
template<class _Rp, class ..._ArgTypes>
template <class _Alloc>
function<_Rp(_ArgTypes...)>::function(allocator_arg_t, const _Alloc&,
                                     function&& __f)
{
    if (__f.__f_ == 0)
        __f_ = 0;
    else if ((void *)__f.__f_ == &__f.__buf_)
    {
        __f_ = __as_base(&__buf_);
        __f.__f_->__clone(__f_);
    }
    else
    {
        __f_ = __f.__f_;
        __f.__f_ = 0;
    }
}
#endif

template<class _Rp, class ..._ArgTypes>
template <class _Fp, class>
function<_Rp(_ArgTypes...)>::function(_Fp __f)
    : __f_(0)
{
    if (__function::__not_null(__f))
    {
        typedef __function::__func<_Fp, allocator<_Fp>, _Rp(_ArgTypes...)> _FF;
        if (sizeof(_FF) <= sizeof(__buf_) && is_nothrow_copy_constructible<_Fp>::value)
        {
            __f_ = ::new((void*)&__buf_) _FF(_VSTD::move(__f));
        }
        else
        {
            typedef allocator<_FF> _Ap;
            _Ap __a;
            typedef __allocator_destructor<_Ap> _Dp;
            unique_ptr<__base, _Dp> __hold(__a.allocate(1), _Dp(__a, 1));
            ::new (__hold.get()) _FF(_VSTD::move(__f), allocator<_Fp>(__a));
            __f_ = __hold.release();
        }
    }
}

#if _LIBCPP_STD_VER <= 14
template<class _Rp, class ..._ArgTypes>
template <class _Fp, class _Alloc, class>
function<_Rp(_ArgTypes...)>::function(allocator_arg_t, const _Alloc& __a0, _Fp __f)
    : __f_(0)
{
    typedef allocator_traits<_Alloc> __alloc_traits;
    if (__function::__not_null(__f))
    {
        typedef __function::__func<_Fp, _Alloc, _Rp(_ArgTypes...)> _FF;
        typedef typename __rebind_alloc_helper<__alloc_traits, _FF>::type _Ap;
        _Ap __a(__a0);
        if (sizeof(_FF) <= sizeof(__buf_) &&
            is_nothrow_copy_constructible<_Fp>::value && is_nothrow_copy_constructible<_Ap>::value)
        {
            __f_ = ::new((void*)&__buf_) _FF(_VSTD::move(__f), _Alloc(__a));
        }
        else
        {
            typedef __allocator_destructor<_Ap> _Dp;
            unique_ptr<__base, _Dp> __hold(__a.allocate(1), _Dp(__a, 1));
            ::new (__hold.get()) _FF(_VSTD::move(__f), _Alloc(__a));
            __f_ = __hold.release();
        }
    }
}
#endif

template<class _Rp, class ..._ArgTypes>
function<_Rp(_ArgTypes...)>&
function<_Rp(_ArgTypes...)>::operator=(const function& __f)
{
    function(__f).swap(*this);
    return *this;
}

template<class _Rp, class ..._ArgTypes>
function<_Rp(_ArgTypes...)>&
function<_Rp(_ArgTypes...)>::operator=(function&& __f) _NOEXCEPT
{
    if ((void *)__f_ == &__buf_)
        __f_->destroy();
    else if (__f_)
        __f_->destroy_deallocate();
    __f_ = 0;
    if (__f.__f_ == 0)
        __f_ = 0;
    else if ((void *)__f.__f_ == &__f.__buf_)
    {
        __f_ = __as_base(&__buf_);
        __f.__f_->__clone(__f_);
    }
    else
    {
        __f_ = __f.__f_;
        __f.__f_ = 0;
    }
    return *this;
}

template<class _Rp, class ..._ArgTypes>
function<_Rp(_ArgTypes...)>&
function<_Rp(_ArgTypes...)>::operator=(nullptr_t) _NOEXCEPT
{
    if ((void *)__f_ == &__buf_)
        __f_->destroy();
    else if (__f_)
        __f_->destroy_deallocate();
    __f_ = 0;
    return *this;
}

template<class _Rp, class ..._ArgTypes>
template <class _Fp, class>
function<_Rp(_ArgTypes...)>&
function<_Rp(_ArgTypes...)>::operator=(_Fp&& __f)
{
    function(_VSTD::forward<_Fp>(__f)).swap(*this);
    return *this;
}

template<class _Rp, class ..._ArgTypes>
function<_Rp(_ArgTypes...)>::~function()
{
    if ((void *)__f_ == &__buf_)
        __f_->destroy();
    else if (__f_)
        __f_->destroy_deallocate();
}

template<class _Rp, class ..._ArgTypes>
void
function<_Rp(_ArgTypes...)>::swap(function& __f) _NOEXCEPT
{
    if (_VSTD::addressof(__f) == this)
      return;
    if ((void *)__f_ == &__buf_ && (void *)__f.__f_ == &__f.__buf_)
    {
        typename aligned_storage<sizeof(__buf_)>::type __tempbuf;
        __base* __t = __as_base(&__tempbuf);
        __f_->__clone(__t);
        __f_->destroy();
        __f_ = 0;
        __f.__f_->__clone(__as_base(&__buf_));
        __f.__f_->destroy();
        __f.__f_ = 0;
        __f_ = __as_base(&__buf_);
        __t->__clone(__as_base(&__f.__buf_));
        __t->destroy();
        __f.__f_ = __as_base(&__f.__buf_);
    }
    else if ((void *)__f_ == &__buf_)
    {
        __f_->__clone(__as_base(&__f.__buf_));
        __f_->destroy();
        __f_ = __f.__f_;
        __f.__f_ = __as_base(&__f.__buf_);
    }
    else if ((void *)__f.__f_ == &__f.__buf_)
    {
        __f.__f_->__clone(__as_base(&__buf_));
        __f.__f_->destroy();
        __f.__f_ = __f_;
        __f_ = __as_base(&__buf_);
    }
    else
        _VSTD::swap(__f_, __f.__f_);
}

template<class _Rp, class ..._ArgTypes>
_Rp
function<_Rp(_ArgTypes...)>::operator()(_ArgTypes... __arg) const
{
    if (__f_ == 0)
        __throw_bad_function_call();
    return (*__f_)(_VSTD::forward<_ArgTypes>(__arg)...);
}

#ifndef _LIBCPP_NO_RTTI

template<class _Rp, class ..._ArgTypes>
const std::type_info&
function<_Rp(_ArgTypes...)>::target_type() const _NOEXCEPT
{
    if (__f_ == 0)
        return typeid(void);
    return __f_->target_type();
}

template<class _Rp, class ..._ArgTypes>
template <typename _Tp>
_Tp*
function<_Rp(_ArgTypes...)>::target() _NOEXCEPT
{
    if (__f_ == 0)
        return nullptr;
    return (_Tp*) const_cast<void *>(__f_->target(typeid(_Tp)));
}

template<class _Rp, class ..._ArgTypes>
template <typename _Tp>
const _Tp*
function<_Rp(_ArgTypes...)>::target() const _NOEXCEPT
{
    if (__f_ == 0)
        return nullptr;
    return (const _Tp*)__f_->target(typeid(_Tp));
}

#endif  // _LIBCPP_NO_RTTI

template <class _Rp, class... _ArgTypes>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator==(const function<_Rp(_ArgTypes...)>& __f, nullptr_t) _NOEXCEPT {return !__f;}

template <class _Rp, class... _ArgTypes>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator==(nullptr_t, const function<_Rp(_ArgTypes...)>& __f) _NOEXCEPT {return !__f;}

template <class _Rp, class... _ArgTypes>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator!=(const function<_Rp(_ArgTypes...)>& __f, nullptr_t) _NOEXCEPT {return (bool)__f;}

template <class _Rp, class... _ArgTypes>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator!=(nullptr_t, const function<_Rp(_ArgTypes...)>& __f) _NOEXCEPT {return (bool)__f;}

template <class _Rp, class... _ArgTypes>
inline _LIBCPP_INLINE_VISIBILITY
void
swap(function<_Rp(_ArgTypes...)>& __x, function<_Rp(_ArgTypes...)>& __y) _NOEXCEPT
{return __x.swap(__y);}

#else // _LIBCPP_CXX03_LANG

#include <__functional_03>

#endif

////////////////////////////////////////////////////////////////////////////////
//                                  BIND
//==============================================================================

template<class _Tp> struct __is_bind_expression : public false_type {};
template<class _Tp> struct _LIBCPP_TEMPLATE_VIS is_bind_expression
    : public __is_bind_expression<typename remove_cv<_Tp>::type> {};

#if _LIBCPP_STD_VER > 14
template <class _Tp>
constexpr size_t is_bind_expression_v = is_bind_expression<_Tp>::value;
#endif

template<class _Tp> struct __is_placeholder : public integral_constant<int, 0> {};
template<class _Tp> struct _LIBCPP_TEMPLATE_VIS is_placeholder
    : public __is_placeholder<typename remove_cv<_Tp>::type> {};

#if _LIBCPP_STD_VER > 14
template <class _Tp>
constexpr size_t is_placeholder_v = is_placeholder<_Tp>::value;
#endif

namespace placeholders
{

template <int _Np> struct __ph {};

#if defined(_LIBCPP_CXX03_LANG) || defined(_LIBCPP_BUILDING_BIND)
_LIBCPP_FUNC_VIS extern const __ph<1>   _1;
_LIBCPP_FUNC_VIS extern const __ph<2>   _2;
_LIBCPP_FUNC_VIS extern const __ph<3>   _3;
_LIBCPP_FUNC_VIS extern const __ph<4>   _4;
_LIBCPP_FUNC_VIS extern const __ph<5>   _5;
_LIBCPP_FUNC_VIS extern const __ph<6>   _6;
_LIBCPP_FUNC_VIS extern const __ph<7>   _7;
_LIBCPP_FUNC_VIS extern const __ph<8>   _8;
_LIBCPP_FUNC_VIS extern const __ph<9>   _9;
_LIBCPP_FUNC_VIS extern const __ph<10> _10;
#else
constexpr __ph<1>   _1{};
constexpr __ph<2>   _2{};
constexpr __ph<3>   _3{};
constexpr __ph<4>   _4{};
constexpr __ph<5>   _5{};
constexpr __ph<6>   _6{};
constexpr __ph<7>   _7{};
constexpr __ph<8>   _8{};
constexpr __ph<9>   _9{};
constexpr __ph<10> _10{};
#endif // defined(_LIBCPP_CXX03_LANG) || defined(_LIBCPP_BUILDING_BIND)

}  // placeholders

template<int _Np>
struct __is_placeholder<placeholders::__ph<_Np> >
    : public integral_constant<int, _Np> {};


#ifndef _LIBCPP_CXX03_LANG

template <class _Tp, class _Uj>
inline _LIBCPP_INLINE_VISIBILITY
_Tp&
__mu(reference_wrapper<_Tp> __t, _Uj&)
{
    return __t.get();
}

template <class _Ti, class ..._Uj, size_t ..._Indx>
inline _LIBCPP_INLINE_VISIBILITY
typename __invoke_of<_Ti&, _Uj...>::type
__mu_expand(_Ti& __ti, tuple<_Uj...>& __uj, __tuple_indices<_Indx...>)
{
    return __ti(_VSTD::forward<_Uj>(_VSTD::get<_Indx>(__uj))...);
}

template <class _Ti, class ..._Uj>
inline _LIBCPP_INLINE_VISIBILITY
typename __lazy_enable_if
<
    is_bind_expression<_Ti>::value,
    __invoke_of<_Ti&, _Uj...>
>::type
__mu(_Ti& __ti, tuple<_Uj...>& __uj)
{
    typedef typename __make_tuple_indices<sizeof...(_Uj)>::type __indices;
    return  __mu_expand(__ti, __uj, __indices());
}

template <bool IsPh, class _Ti, class _Uj>
struct __mu_return2 {};

template <class _Ti, class _Uj>
struct __mu_return2<true, _Ti, _Uj>
{
    typedef typename tuple_element<is_placeholder<_Ti>::value - 1, _Uj>::type type;
};

template <class _Ti, class _Uj>
inline _LIBCPP_INLINE_VISIBILITY
typename enable_if
<
    0 < is_placeholder<_Ti>::value,
    typename __mu_return2<0 < is_placeholder<_Ti>::value, _Ti, _Uj>::type
>::type
__mu(_Ti&, _Uj& __uj)
{
    const size_t _Indx = is_placeholder<_Ti>::value - 1;
    return _VSTD::forward<typename tuple_element<_Indx, _Uj>::type>(_VSTD::get<_Indx>(__uj));
}

template <class _Ti, class _Uj>
inline _LIBCPP_INLINE_VISIBILITY
typename enable_if
<
    !is_bind_expression<_Ti>::value &&
    is_placeholder<_Ti>::value == 0 &&
    !__is_reference_wrapper<_Ti>::value,
    _Ti&
>::type
__mu(_Ti& __ti, _Uj&)
{
    return __ti;
}

template <class _Ti, bool IsReferenceWrapper, bool IsBindEx, bool IsPh,
          class _TupleUj>
struct ____mu_return;

template <bool _Invokable, class _Ti, class ..._Uj>
struct ____mu_return_invokable  // false
{
    typedef __nat type;
};

template <class _Ti, class ..._Uj>
struct ____mu_return_invokable<true, _Ti, _Uj...>
{
    typedef typename __invoke_of<_Ti&, _Uj...>::type type;
};

template <class _Ti, class ..._Uj>
struct ____mu_return<_Ti, false, true, false, tuple<_Uj...> >
    : public ____mu_return_invokable<__invokable<_Ti&, _Uj...>::value, _Ti, _Uj...>
{
};

template <class _Ti, class _TupleUj>
struct ____mu_return<_Ti, false, false, true, _TupleUj>
{
    typedef typename tuple_element<is_placeholder<_Ti>::value - 1,
                                   _TupleUj>::type&& type;
};

template <class _Ti, class _TupleUj>
struct ____mu_return<_Ti, true, false, false, _TupleUj>
{
    typedef typename _Ti::type& type;
};

template <class _Ti, class _TupleUj>
struct ____mu_return<_Ti, false, false, false, _TupleUj>
{
    typedef _Ti& type;
};

template <class _Ti, class _TupleUj>
struct __mu_return
    : public ____mu_return<_Ti,
                           __is_reference_wrapper<_Ti>::value,
                           is_bind_expression<_Ti>::value,
                           0 < is_placeholder<_Ti>::value &&
                           is_placeholder<_Ti>::value <= tuple_size<_TupleUj>::value,
                           _TupleUj>
{
};

template <class _Fp, class _BoundArgs, class _TupleUj>
struct __is_valid_bind_return
{
    static const bool value = false;
};

template <class _Fp, class ..._BoundArgs, class _TupleUj>
struct __is_valid_bind_return<_Fp, tuple<_BoundArgs...>, _TupleUj>
{
    static const bool value = __invokable<_Fp,
                    typename __mu_return<_BoundArgs, _TupleUj>::type...>::value;
};

template <class _Fp, class ..._BoundArgs, class _TupleUj>
struct __is_valid_bind_return<_Fp, const tuple<_BoundArgs...>, _TupleUj>
{
    static const bool value = __invokable<_Fp,
                    typename __mu_return<const _BoundArgs, _TupleUj>::type...>::value;
};

template <class _Fp, class _BoundArgs, class _TupleUj,
          bool = __is_valid_bind_return<_Fp, _BoundArgs, _TupleUj>::value>
struct __bind_return;

template <class _Fp, class ..._BoundArgs, class _TupleUj>
struct __bind_return<_Fp, tuple<_BoundArgs...>, _TupleUj, true>
{
    typedef typename __invoke_of
    <
        _Fp&,
        typename __mu_return
        <
            _BoundArgs,
            _TupleUj
        >::type...
    >::type type;
};

template <class _Fp, class ..._BoundArgs, class _TupleUj>
struct __bind_return<_Fp, const tuple<_BoundArgs...>, _TupleUj, true>
{
    typedef typename __invoke_of
    <
        _Fp&,
        typename __mu_return
        <
            const _BoundArgs,
            _TupleUj
        >::type...
    >::type type;
};

template <class _Fp, class _BoundArgs, size_t ..._Indx, class _Args>
inline _LIBCPP_INLINE_VISIBILITY
typename __bind_return<_Fp, _BoundArgs, _Args>::type
__apply_functor(_Fp& __f, _BoundArgs& __bound_args, __tuple_indices<_Indx...>,
                _Args&& __args)
{
    return _VSTD::__invoke(__f, _VSTD::__mu(_VSTD::get<_Indx>(__bound_args), __args)...);
}

template<class _Fp, class ..._BoundArgs>
class __bind
    : public __weak_result_type<typename decay<_Fp>::type>
{
protected:
    typedef typename decay<_Fp>::type _Fd;
    typedef tuple<typename decay<_BoundArgs>::type...> _Td;
private:
    _Fd __f_;
    _Td __bound_args_;

    typedef typename __make_tuple_indices<sizeof...(_BoundArgs)>::type __indices;
public:
    template <class _Gp, class ..._BA,
              class = typename enable_if
                               <
                                  is_constructible<_Fd, _Gp>::value &&
                                  !is_same<typename remove_reference<_Gp>::type,
                                           __bind>::value
                               >::type>
      _LIBCPP_INLINE_VISIBILITY
      explicit __bind(_Gp&& __f, _BA&& ...__bound_args)
        : __f_(_VSTD::forward<_Gp>(__f)),
          __bound_args_(_VSTD::forward<_BA>(__bound_args)...) {}

    template <class ..._Args>
        _LIBCPP_INLINE_VISIBILITY
        typename __bind_return<_Fd, _Td, tuple<_Args&&...> >::type
        operator()(_Args&& ...__args)
        {
            return _VSTD::__apply_functor(__f_, __bound_args_, __indices(),
                                  tuple<_Args&&...>(_VSTD::forward<_Args>(__args)...));
        }

    template <class ..._Args>
        _LIBCPP_INLINE_VISIBILITY
        typename __bind_return<const _Fd, const _Td, tuple<_Args&&...> >::type
        operator()(_Args&& ...__args) const
        {
            return _VSTD::__apply_functor(__f_, __bound_args_, __indices(),
                                   tuple<_Args&&...>(_VSTD::forward<_Args>(__args)...));
        }
};

template<class _Fp, class ..._BoundArgs>
struct __is_bind_expression<__bind<_Fp, _BoundArgs...> > : public true_type {};

template<class _Rp, class _Fp, class ..._BoundArgs>
class __bind_r
    : public __bind<_Fp, _BoundArgs...>
{
    typedef __bind<_Fp, _BoundArgs...> base;
    typedef typename base::_Fd _Fd;
    typedef typename base::_Td _Td;
public:
    typedef _Rp result_type;


    template <class _Gp, class ..._BA,
              class = typename enable_if
                               <
                                  is_constructible<_Fd, _Gp>::value &&
                                  !is_same<typename remove_reference<_Gp>::type,
                                           __bind_r>::value
                               >::type>
      _LIBCPP_INLINE_VISIBILITY
      explicit __bind_r(_Gp&& __f, _BA&& ...__bound_args)
        : base(_VSTD::forward<_Gp>(__f),
               _VSTD::forward<_BA>(__bound_args)...) {}

    template <class ..._Args>
        _LIBCPP_INLINE_VISIBILITY
        typename enable_if
        <
            is_convertible<typename __bind_return<_Fd, _Td, tuple<_Args&&...> >::type,
                           result_type>::value || is_void<_Rp>::value,
            result_type
        >::type
        operator()(_Args&& ...__args)
        {
            typedef __invoke_void_return_wrapper<_Rp> _Invoker;
            return _Invoker::__call(static_cast<base&>(*this), _VSTD::forward<_Args>(__args)...);
        }

    template <class ..._Args>
        _LIBCPP_INLINE_VISIBILITY
        typename enable_if
        <
            is_convertible<typename __bind_return<const _Fd, const _Td, tuple<_Args&&...> >::type,
                           result_type>::value || is_void<_Rp>::value,
            result_type
        >::type
        operator()(_Args&& ...__args) const
        {
            typedef __invoke_void_return_wrapper<_Rp> _Invoker;
            return _Invoker::__call(static_cast<base const&>(*this), _VSTD::forward<_Args>(__args)...);
        }
};

template<class _Rp, class _Fp, class ..._BoundArgs>
struct __is_bind_expression<__bind_r<_Rp, _Fp, _BoundArgs...> > : public true_type {};

template<class _Fp, class ..._BoundArgs>
inline _LIBCPP_INLINE_VISIBILITY
__bind<_Fp, _BoundArgs...>
bind(_Fp&& __f, _BoundArgs&&... __bound_args)
{
    typedef __bind<_Fp, _BoundArgs...> type;
    return type(_VSTD::forward<_Fp>(__f), _VSTD::forward<_BoundArgs>(__bound_args)...);
}

template<class _Rp, class _Fp, class ..._BoundArgs>
inline _LIBCPP_INLINE_VISIBILITY
__bind_r<_Rp, _Fp, _BoundArgs...>
bind(_Fp&& __f, _BoundArgs&&... __bound_args)
{
    typedef __bind_r<_Rp, _Fp, _BoundArgs...> type;
    return type(_VSTD::forward<_Fp>(__f), _VSTD::forward<_BoundArgs>(__bound_args)...);
}

#endif  // _LIBCPP_CXX03_LANG

#if _LIBCPP_STD_VER > 14

#define __cpp_lib_invoke 201411

template <class _Fn, class ..._Args>
result_of_t<_Fn&&(_Args&&...)>
invoke(_Fn&& __f, _Args&&... __args)
    noexcept(noexcept(_VSTD::__invoke(_VSTD::forward<_Fn>(__f), _VSTD::forward<_Args>(__args)...)))
{
    return _VSTD::__invoke(_VSTD::forward<_Fn>(__f), _VSTD::forward<_Args>(__args)...);
}

template <class _DecayFunc>
class _LIBCPP_TEMPLATE_VIS __not_fn_imp {
  _DecayFunc __fd;

public:
    __not_fn_imp() = delete;

    template <class ..._Args>
    _LIBCPP_INLINE_VISIBILITY
    auto operator()(_Args&& ...__args) &
            noexcept(noexcept(!_VSTD::invoke(__fd, _VSTD::forward<_Args>(__args)...)))
        -> decltype(          !_VSTD::invoke(__fd, _VSTD::forward<_Args>(__args)...))
        { return              !_VSTD::invoke(__fd, _VSTD::forward<_Args>(__args)...); }

    template <class ..._Args>
    _LIBCPP_INLINE_VISIBILITY
    auto operator()(_Args&& ...__args) &&
            noexcept(noexcept(!_VSTD::invoke(_VSTD::move(__fd), _VSTD::forward<_Args>(__args)...)))
        -> decltype(          !_VSTD::invoke(_VSTD::move(__fd), _VSTD::forward<_Args>(__args)...))
        { return              !_VSTD::invoke(_VSTD::move(__fd), _VSTD::forward<_Args>(__args)...); }

    template <class ..._Args>
    _LIBCPP_INLINE_VISIBILITY
    auto operator()(_Args&& ...__args) const&
            noexcept(noexcept(!_VSTD::invoke(__fd, _VSTD::forward<_Args>(__args)...)))
        -> decltype(          !_VSTD::invoke(__fd, _VSTD::forward<_Args>(__args)...))
        { return              !_VSTD::invoke(__fd, _VSTD::forward<_Args>(__args)...); }


    template <class ..._Args>
    _LIBCPP_INLINE_VISIBILITY
    auto operator()(_Args&& ...__args) const&&
            noexcept(noexcept(!_VSTD::invoke(_VSTD::move(__fd), _VSTD::forward<_Args>(__args)...)))
        -> decltype(          !_VSTD::invoke(_VSTD::move(__fd), _VSTD::forward<_Args>(__args)...))
        { return              !_VSTD::invoke(_VSTD::move(__fd), _VSTD::forward<_Args>(__args)...); }

private:
    template <class _RawFunc,
              class = enable_if_t<!is_same<decay_t<_RawFunc>, __not_fn_imp>::value>>
    _LIBCPP_INLINE_VISIBILITY
    explicit __not_fn_imp(_RawFunc&& __rf)
        : __fd(_VSTD::forward<_RawFunc>(__rf)) {}

    template <class _RawFunc>
    friend inline _LIBCPP_INLINE_VISIBILITY
    __not_fn_imp<decay_t<_RawFunc>> not_fn(_RawFunc&&);
};

template <class _RawFunc>
inline _LIBCPP_INLINE_VISIBILITY
__not_fn_imp<decay_t<_RawFunc>> not_fn(_RawFunc&& __fn) {
    return __not_fn_imp<decay_t<_RawFunc>>(_VSTD::forward<_RawFunc>(__fn));
}

#endif

// struct hash<T*> in <memory>

_LIBCPP_END_NAMESPACE_STD

#endif  // _LIBCPP_FUNCTIONAL
