// -*- C++ -*-
//===--------------------------- tuple ------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_TUPLE
#define _LIBCPP_TUPLE

#include "__config.hpp"
#include "__tuple.hpp"
#include <stddef.h>
#include "type_traits.hpp"
#include "__functional_base.hpp"
#include "utility.hpp"

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

#ifndef _LIBCPP_CXX03_LANG


// __tuple_leaf

template <size_t _Ip, class _Hp,
          bool=is_empty<_Hp>::value && !__libcpp_is_final<_Hp>::value
         >
class __tuple_leaf;

template <size_t _Ip, class _Hp, bool _Ep>
inline _LIBCPP_INLINE_VISIBILITY
void swap(__tuple_leaf<_Ip, _Hp, _Ep>& __x, __tuple_leaf<_Ip, _Hp, _Ep>& __y)
    _NOEXCEPT_(__is_nothrow_swappable<_Hp>::value)
{
    swap(__x.get(), __y.get());
}

template <size_t _Ip, class _Hp, bool>
class __tuple_leaf
{
    _Hp __value_;

    template <class _Tp>
    static constexpr bool __can_bind_reference() {
        using _RawTp = typename remove_reference<_Tp>::type;
        using _RawHp = typename remove_reference<_Hp>::type;
        using _CheckLValueArg = integral_constant<bool,
            is_lvalue_reference<_Tp>::value
        ||  is_same<_RawTp, reference_wrapper<_RawHp>>::value
        ||  is_same<_RawTp, reference_wrapper<typename remove_const<_RawHp>::type>>::value
        >;
        return  !is_reference<_Hp>::value
            || (is_lvalue_reference<_Hp>::value && _CheckLValueArg::value)
            || (is_rvalue_reference<_Hp>::value && !is_lvalue_reference<_Tp>::value);
    }

    __tuple_leaf& operator=(const __tuple_leaf&);
public:
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR __tuple_leaf()
             _NOEXCEPT_(is_nothrow_default_constructible<_Hp>::value) : __value_()
       {static_assert(!is_reference<_Hp>::value,
              "Attempted to default construct a reference element in a tuple");}

    template <class _Alloc>
        _LIBCPP_INLINE_VISIBILITY
        __tuple_leaf(integral_constant<int, 0>, const _Alloc&)
            : __value_()
        {static_assert(!is_reference<_Hp>::value,
              "Attempted to default construct a reference element in a tuple");}

    template <class _Alloc>
        _LIBCPP_INLINE_VISIBILITY
        __tuple_leaf(integral_constant<int, 1>, const _Alloc& __a)
            : __value_(allocator_arg_t(), __a)
        {static_assert(!is_reference<_Hp>::value,
              "Attempted to default construct a reference element in a tuple");}

    template <class _Alloc>
        _LIBCPP_INLINE_VISIBILITY
        __tuple_leaf(integral_constant<int, 2>, const _Alloc& __a)
            : __value_(__a)
        {static_assert(!is_reference<_Hp>::value,
              "Attempted to default construct a reference element in a tuple");}

    template <class _Tp,
              class = typename enable_if<
                  __lazy_and<
                      __lazy_not<is_same<typename decay<_Tp>::type, __tuple_leaf>>
                    , is_constructible<_Hp, _Tp>
                    >::value
                >::type
            >
        _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
        explicit __tuple_leaf(_Tp&& __t) _NOEXCEPT_((is_nothrow_constructible<_Hp, _Tp>::value))
            : __value_(_VSTD::forward<_Tp>(__t))
        {static_assert(__can_bind_reference<_Tp>(),
       "Attempted to construct a reference element in a tuple with an rvalue");}

    template <class _Tp, class _Alloc>
        _LIBCPP_INLINE_VISIBILITY
        explicit __tuple_leaf(integral_constant<int, 0>, const _Alloc&, _Tp&& __t)
            : __value_(_VSTD::forward<_Tp>(__t))
        {static_assert(__can_bind_reference<_Tp>(),
       "Attempted to construct a reference element in a tuple with an rvalue");}

    template <class _Tp, class _Alloc>
        _LIBCPP_INLINE_VISIBILITY
        explicit __tuple_leaf(integral_constant<int, 1>, const _Alloc& __a, _Tp&& __t)
            : __value_(allocator_arg_t(), __a, _VSTD::forward<_Tp>(__t))
        {static_assert(!is_reference<_Hp>::value,
            "Attempted to uses-allocator construct a reference element in a tuple");}

    template <class _Tp, class _Alloc>
        _LIBCPP_INLINE_VISIBILITY
        explicit __tuple_leaf(integral_constant<int, 2>, const _Alloc& __a, _Tp&& __t)
            : __value_(_VSTD::forward<_Tp>(__t), __a)
        {static_assert(!is_reference<_Hp>::value,
           "Attempted to uses-allocator construct a reference element in a tuple");}

    __tuple_leaf(const __tuple_leaf& __t) = default;
    __tuple_leaf(__tuple_leaf&& __t) = default;

    template <class _Tp>
        _LIBCPP_INLINE_VISIBILITY
        __tuple_leaf&
        operator=(_Tp&& __t) _NOEXCEPT_((is_nothrow_assignable<_Hp&, _Tp>::value))
        {
            __value_ = _VSTD::forward<_Tp>(__t);
            return *this;
        }

    _LIBCPP_INLINE_VISIBILITY
    int swap(__tuple_leaf& __t) _NOEXCEPT_(__is_nothrow_swappable<__tuple_leaf>::value)
    {
        _VSTD::swap(*this, __t);
        return 0;
    }

    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11       _Hp& get()       _NOEXCEPT {return __value_;}
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11 const _Hp& get() const _NOEXCEPT {return __value_;}
};

template <size_t _Ip, class _Hp>
class __tuple_leaf<_Ip, _Hp, true>
    : private _Hp
{

    __tuple_leaf& operator=(const __tuple_leaf&);
public:
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR __tuple_leaf()
             _NOEXCEPT_(is_nothrow_default_constructible<_Hp>::value) {}

    template <class _Alloc>
        _LIBCPP_INLINE_VISIBILITY
        __tuple_leaf(integral_constant<int, 0>, const _Alloc&) {}

    template <class _Alloc>
        _LIBCPP_INLINE_VISIBILITY
        __tuple_leaf(integral_constant<int, 1>, const _Alloc& __a)
            : _Hp(allocator_arg_t(), __a) {}

    template <class _Alloc>
        _LIBCPP_INLINE_VISIBILITY
        __tuple_leaf(integral_constant<int, 2>, const _Alloc& __a)
            : _Hp(__a) {}

    template <class _Tp,
              class = typename enable_if<
                  __lazy_and<
                        __lazy_not<is_same<typename decay<_Tp>::type, __tuple_leaf>>
                      , is_constructible<_Hp, _Tp>
                    >::value
                >::type
            >
        _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
        explicit __tuple_leaf(_Tp&& __t) _NOEXCEPT_((is_nothrow_constructible<_Hp, _Tp>::value))
            : _Hp(_VSTD::forward<_Tp>(__t)) {}

    template <class _Tp, class _Alloc>
        _LIBCPP_INLINE_VISIBILITY
        explicit __tuple_leaf(integral_constant<int, 0>, const _Alloc&, _Tp&& __t)
            : _Hp(_VSTD::forward<_Tp>(__t)) {}

    template <class _Tp, class _Alloc>
        _LIBCPP_INLINE_VISIBILITY
        explicit __tuple_leaf(integral_constant<int, 1>, const _Alloc& __a, _Tp&& __t)
            : _Hp(allocator_arg_t(), __a, _VSTD::forward<_Tp>(__t)) {}

    template <class _Tp, class _Alloc>
        _LIBCPP_INLINE_VISIBILITY
        explicit __tuple_leaf(integral_constant<int, 2>, const _Alloc& __a, _Tp&& __t)
            : _Hp(_VSTD::forward<_Tp>(__t), __a) {}

    __tuple_leaf(__tuple_leaf const &) = default;
    __tuple_leaf(__tuple_leaf &&) = default;

    template <class _Tp>
        _LIBCPP_INLINE_VISIBILITY
        __tuple_leaf&
        operator=(_Tp&& __t) _NOEXCEPT_((is_nothrow_assignable<_Hp&, _Tp>::value))
        {
            _Hp::operator=(_VSTD::forward<_Tp>(__t));
            return *this;
        }

    _LIBCPP_INLINE_VISIBILITY
    int
    swap(__tuple_leaf& __t) _NOEXCEPT_(__is_nothrow_swappable<__tuple_leaf>::value)
    {
        _VSTD::swap(*this, __t);
        return 0;
    }

    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11       _Hp& get()       _NOEXCEPT {return static_cast<_Hp&>(*this);}
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11 const _Hp& get() const _NOEXCEPT {return static_cast<const _Hp&>(*this);}
};

template <class ..._Tp>
_LIBCPP_INLINE_VISIBILITY
void __swallow(_Tp&&...) _NOEXCEPT {}

template <class ..._Tp>
struct __lazy_all : __all<_Tp::value...> {};

template <class _Tp>
struct __all_default_constructible;

template <class ..._Tp>
struct __all_default_constructible<__tuple_types<_Tp...>>
    : __all<is_default_constructible<_Tp>::value...>
{ };

// __tuple_impl

template<class _Indx, class ..._Tp> struct __tuple_impl;

template<size_t ..._Indx, class ..._Tp>
struct _LIBCPP_DECLSPEC_EMPTY_BASES __tuple_impl<__tuple_indices<_Indx...>, _Tp...>
    : public __tuple_leaf<_Indx, _Tp>...
{
    _LIBCPP_INLINE_VISIBILITY
    _LIBCPP_CONSTEXPR __tuple_impl()
        _NOEXCEPT_(__all<is_nothrow_default_constructible<_Tp>::value...>::value) {}

    template <size_t ..._Uf, class ..._Tf,
              size_t ..._Ul, class ..._Tl, class ..._Up>
        _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
        explicit
        __tuple_impl(__tuple_indices<_Uf...>, __tuple_types<_Tf...>,
                     __tuple_indices<_Ul...>, __tuple_types<_Tl...>,
                     _Up&&... __u)
                     _NOEXCEPT_((__all<is_nothrow_constructible<_Tf, _Up>::value...>::value &&
                                 __all<is_nothrow_default_constructible<_Tl>::value...>::value)) :
            __tuple_leaf<_Uf, _Tf>(_VSTD::forward<_Up>(__u))...,
            __tuple_leaf<_Ul, _Tl>()...
            {}

    template <class _Alloc, size_t ..._Uf, class ..._Tf,
              size_t ..._Ul, class ..._Tl, class ..._Up>
        _LIBCPP_INLINE_VISIBILITY
        explicit
        __tuple_impl(allocator_arg_t, const _Alloc& __a,
                     __tuple_indices<_Uf...>, __tuple_types<_Tf...>,
                     __tuple_indices<_Ul...>, __tuple_types<_Tl...>,
                     _Up&&... __u) :
            __tuple_leaf<_Uf, _Tf>(__uses_alloc_ctor<_Tf, _Alloc, _Up>(), __a,
            _VSTD::forward<_Up>(__u))...,
            __tuple_leaf<_Ul, _Tl>(__uses_alloc_ctor<_Tl, _Alloc>(), __a)...
            {}

    template <class _Tuple,
              class = typename enable_if
                      <
                         __tuple_constructible<_Tuple, tuple<_Tp...> >::value
                      >::type
             >
        _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
        __tuple_impl(_Tuple&& __t) _NOEXCEPT_((__all<is_nothrow_constructible<_Tp, typename tuple_element<_Indx,
                                       typename __make_tuple_types<_Tuple>::type>::type>::value...>::value))
            : __tuple_leaf<_Indx, _Tp>(_VSTD::forward<typename tuple_element<_Indx,
                                       typename __make_tuple_types<_Tuple>::type>::type>(_VSTD::get<_Indx>(__t)))...
            {}

    template <class _Alloc, class _Tuple,
              class = typename enable_if
                      <
                         __tuple_constructible<_Tuple, tuple<_Tp...> >::value
                      >::type
             >
        _LIBCPP_INLINE_VISIBILITY
        __tuple_impl(allocator_arg_t, const _Alloc& __a, _Tuple&& __t)
            : __tuple_leaf<_Indx, _Tp>(__uses_alloc_ctor<_Tp, _Alloc, typename tuple_element<_Indx,
                                       typename __make_tuple_types<_Tuple>::type>::type>(), __a,
                                       _VSTD::forward<typename tuple_element<_Indx,
                                       typename __make_tuple_types<_Tuple>::type>::type>(_VSTD::get<_Indx>(__t)))...
            {}

    template <class _Tuple>
        _LIBCPP_INLINE_VISIBILITY
        typename enable_if
        <
            __tuple_assignable<_Tuple, tuple<_Tp...> >::value,
            __tuple_impl&
        >::type
        operator=(_Tuple&& __t) _NOEXCEPT_((__all<is_nothrow_assignable<_Tp&, typename tuple_element<_Indx,
                                       typename __make_tuple_types<_Tuple>::type>::type>::value...>::value))
        {
            __swallow(__tuple_leaf<_Indx, _Tp>::operator=(_VSTD::forward<typename tuple_element<_Indx,
                                       typename __make_tuple_types<_Tuple>::type>::type>(_VSTD::get<_Indx>(__t)))...);
            return *this;
        }

    __tuple_impl(const __tuple_impl&) = default;
    __tuple_impl(__tuple_impl&&) = default;

    _LIBCPP_INLINE_VISIBILITY
    __tuple_impl&
    operator=(const __tuple_impl& __t) _NOEXCEPT_((__all<is_nothrow_copy_assignable<_Tp>::value...>::value))
    {
        __swallow(__tuple_leaf<_Indx, _Tp>::operator=(static_cast<const __tuple_leaf<_Indx, _Tp>&>(__t).get())...);
        return *this;
    }

    _LIBCPP_INLINE_VISIBILITY
    __tuple_impl&
    operator=(__tuple_impl&& __t) _NOEXCEPT_((__all<is_nothrow_move_assignable<_Tp>::value...>::value))
    {
        __swallow(__tuple_leaf<_Indx, _Tp>::operator=(_VSTD::forward<_Tp>(static_cast<__tuple_leaf<_Indx, _Tp>&>(__t).get()))...);
        return *this;
    }

    _LIBCPP_INLINE_VISIBILITY
    void swap(__tuple_impl& __t)
        _NOEXCEPT_(__all<__is_nothrow_swappable<_Tp>::value...>::value)
    {
        __swallow(__tuple_leaf<_Indx, _Tp>::swap(static_cast<__tuple_leaf<_Indx, _Tp>&>(__t))...);
    }
};



template <class ..._Tp>
class _LIBCPP_TEMPLATE_VIS tuple
{
    typedef __tuple_impl<typename __make_tuple_indices<sizeof...(_Tp)>::type, _Tp...> _BaseT;

    _BaseT __base_;

#if defined(_LIBCPP_ENABLE_TUPLE_IMPLICIT_REDUCED_ARITY_EXTENSION)
    static constexpr bool _EnableImplicitReducedArityExtension = true;
#else
    static constexpr bool _EnableImplicitReducedArityExtension = false;
#endif

    template <class ..._Args>
    struct _PackExpandsToThisTuple : false_type {};

    template <class _Arg>
    struct _PackExpandsToThisTuple<_Arg>
        : is_same<typename __uncvref<_Arg>::type, tuple> {};

    template <bool _MaybeEnable, class _Dummy = void>
    struct _CheckArgsConstructor : __check_tuple_constructor_fail {};

    template <class _Dummy>
    struct _CheckArgsConstructor<true, _Dummy>
    {
        template <class ..._Args>
        static constexpr bool __enable_default() {
            return __all<is_default_constructible<_Args>::value...>::value;
        }

        template <class ..._Args>
        static constexpr bool __enable_explicit() {
            return
                __tuple_constructible<
                    tuple<_Args...>,
                    typename __make_tuple_types<tuple,
                             sizeof...(_Args) < sizeof...(_Tp) ?
                                 sizeof...(_Args) :
                                 sizeof...(_Tp)>::type
                >::value &&
                !__tuple_convertible<
                    tuple<_Args...>,
                    typename __make_tuple_types<tuple,
                             sizeof...(_Args) < sizeof...(_Tp) ?
                                 sizeof...(_Args) :
                                 sizeof...(_Tp)>::type
                >::value &&
                __all_default_constructible<
                    typename __make_tuple_types<tuple, sizeof...(_Tp),
                             sizeof...(_Args) < sizeof...(_Tp) ?
                                 sizeof...(_Args) :
                                 sizeof...(_Tp)>::type
                >::value;
        }

        template <class ..._Args>
        static constexpr bool __enable_implicit() {
            return
                __tuple_convertible<
                    tuple<_Args...>,
                    typename __make_tuple_types<tuple,
                             sizeof...(_Args) < sizeof...(_Tp) ?
                                 sizeof...(_Args) :
                                 sizeof...(_Tp)>::type
                >::value &&
                __all_default_constructible<
                    typename __make_tuple_types<tuple, sizeof...(_Tp),
                             sizeof...(_Args) < sizeof...(_Tp) ?
                                 sizeof...(_Args) :
                                 sizeof...(_Tp)>::type
                >::value;
        }
    };

    template <bool _MaybeEnable,
              bool = sizeof...(_Tp) == 1,
              class _Dummy = void>
    struct _CheckTupleLikeConstructor : __check_tuple_constructor_fail {};

    template <class _Dummy>
    struct _CheckTupleLikeConstructor<true, false, _Dummy>
    {
        template <class _Tuple>
        static constexpr bool __enable_implicit() {
            return __tuple_convertible<_Tuple, tuple>::value;
        }

        template <class _Tuple>
        static constexpr bool __enable_explicit() {
            return __tuple_constructible<_Tuple, tuple>::value
               && !__tuple_convertible<_Tuple, tuple>::value;
        }
    };

    template <class _Dummy>
    struct _CheckTupleLikeConstructor<true, true, _Dummy>
    {
        // This trait is used to disable the tuple-like constructor when
        // the UTypes... constructor should be selected instead.
        // See LWG issue #2549.
        template <class _Tuple>
        using _PreferTupleLikeConstructor = __lazy_or<
            // Don't attempt the two checks below if the tuple we are given
            // has the same type as this tuple.
            is_same<typename __uncvref<_Tuple>::type, tuple>,
            __lazy_and<
                __lazy_not<is_constructible<_Tp..., _Tuple>>,
                __lazy_not<is_convertible<_Tuple, _Tp...>>
            >
        >;

        template <class _Tuple>
        static constexpr bool __enable_implicit() {
            return __lazy_and<
                __tuple_convertible<_Tuple, tuple>,
                _PreferTupleLikeConstructor<_Tuple>
            >::value;
        }

        template <class _Tuple>
        static constexpr bool __enable_explicit() {
            return __lazy_and<
                __tuple_constructible<_Tuple, tuple>,
                _PreferTupleLikeConstructor<_Tuple>,
                __lazy_not<__tuple_convertible<_Tuple, tuple>>
            >::value;
        }
    };

    template <size_t _Jp, class ..._Up> friend _LIBCPP_CONSTEXPR_AFTER_CXX11
        typename tuple_element<_Jp, tuple<_Up...> >::type& get(tuple<_Up...>&) _NOEXCEPT;
    template <size_t _Jp, class ..._Up> friend _LIBCPP_CONSTEXPR_AFTER_CXX11
        const typename tuple_element<_Jp, tuple<_Up...> >::type& get(const tuple<_Up...>&) _NOEXCEPT;
    template <size_t _Jp, class ..._Up> friend _LIBCPP_CONSTEXPR_AFTER_CXX11
        typename tuple_element<_Jp, tuple<_Up...> >::type&& get(tuple<_Up...>&&) _NOEXCEPT;
    template <size_t _Jp, class ..._Up> friend _LIBCPP_CONSTEXPR_AFTER_CXX11
        const typename tuple_element<_Jp, tuple<_Up...> >::type&& get(const tuple<_Up...>&&) _NOEXCEPT;
public:

    template <bool _Dummy = true, class = typename enable_if<
        _CheckArgsConstructor<_Dummy>::template __enable_default<_Tp...>()
    >::type>
    _LIBCPP_INLINE_VISIBILITY
    _LIBCPP_CONSTEXPR tuple()
        _NOEXCEPT_(__all<is_nothrow_default_constructible<_Tp>::value...>::value) {}

    tuple(tuple const&) = default;
    tuple(tuple&&) = default;

    template <class _AllocArgT, class _Alloc, bool _Dummy = true, class = typename enable_if<
        __lazy_and<
            is_same<allocator_arg_t, _AllocArgT>,
            __lazy_all<__dependent_type<is_default_constructible<_Tp>, _Dummy>...>
       >::value
    >::type>
    _LIBCPP_INLINE_VISIBILITY
    tuple(_AllocArgT, _Alloc const& __a)
      : __base_(allocator_arg_t(), __a,
                    __tuple_indices<>(), __tuple_types<>(),
                    typename __make_tuple_indices<sizeof...(_Tp), 0>::type(),
                    __tuple_types<_Tp...>()) {}

    template <bool _Dummy = true,
              typename enable_if
                      <
                         _CheckArgsConstructor<
                            _Dummy
                         >::template __enable_implicit<_Tp const&...>(),
                         bool
                      >::type = false
        >
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
    tuple(const _Tp& ... __t) _NOEXCEPT_((__all<is_nothrow_copy_constructible<_Tp>::value...>::value))
        : __base_(typename __make_tuple_indices<sizeof...(_Tp)>::type(),
                typename __make_tuple_types<tuple, sizeof...(_Tp)>::type(),
                typename __make_tuple_indices<0>::type(),
                typename __make_tuple_types<tuple, 0>::type(),
                __t...
               ) {}

    template <bool _Dummy = true,
              typename enable_if
                      <
                         _CheckArgsConstructor<
                            _Dummy
                         >::template __enable_explicit<_Tp const&...>(),
                         bool
                      >::type = false
        >
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
    explicit tuple(const _Tp& ... __t) _NOEXCEPT_((__all<is_nothrow_copy_constructible<_Tp>::value...>::value))
        : __base_(typename __make_tuple_indices<sizeof...(_Tp)>::type(),
                typename __make_tuple_types<tuple, sizeof...(_Tp)>::type(),
                typename __make_tuple_indices<0>::type(),
                typename __make_tuple_types<tuple, 0>::type(),
                __t...
               ) {}

    template <class _Alloc, bool _Dummy = true,
              typename enable_if
                      <
                         _CheckArgsConstructor<
                            _Dummy
                         >::template __enable_implicit<_Tp const&...>(),
                         bool
                      >::type = false
        >
      _LIBCPP_INLINE_VISIBILITY
      tuple(allocator_arg_t, const _Alloc& __a, const _Tp& ... __t)
        : __base_(allocator_arg_t(), __a,
                typename __make_tuple_indices<sizeof...(_Tp)>::type(),
                typename __make_tuple_types<tuple, sizeof...(_Tp)>::type(),
                typename __make_tuple_indices<0>::type(),
                typename __make_tuple_types<tuple, 0>::type(),
                __t...
               ) {}

    template <class _Alloc, bool _Dummy = true,
              typename enable_if
                      <
                         _CheckArgsConstructor<
                            _Dummy
                         >::template __enable_explicit<_Tp const&...>(),
                         bool
                      >::type = false
        >
      _LIBCPP_INLINE_VISIBILITY
      explicit
      tuple(allocator_arg_t, const _Alloc& __a, const _Tp& ... __t)
        : __base_(allocator_arg_t(), __a,
                typename __make_tuple_indices<sizeof...(_Tp)>::type(),
                typename __make_tuple_types<tuple, sizeof...(_Tp)>::type(),
                typename __make_tuple_indices<0>::type(),
                typename __make_tuple_types<tuple, 0>::type(),
                __t...
               ) {}

    template <class ..._Up,
              bool _PackIsTuple = _PackExpandsToThisTuple<_Up...>::value,
              typename enable_if
                      <
                         _CheckArgsConstructor<
                             sizeof...(_Up) == sizeof...(_Tp)
                             && !_PackIsTuple
                         >::template __enable_implicit<_Up...>() ||
                        _CheckArgsConstructor<
                            _EnableImplicitReducedArityExtension
                            && sizeof...(_Up) < sizeof...(_Tp)
                            && !_PackIsTuple
                         >::template __enable_implicit<_Up...>(),
                         bool
                      >::type = false
             >
        _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
        tuple(_Up&&... __u)
            _NOEXCEPT_((
                is_nothrow_constructible<_BaseT,
                    typename __make_tuple_indices<sizeof...(_Up)>::type,
                    typename __make_tuple_types<tuple, sizeof...(_Up)>::type,
                    typename __make_tuple_indices<sizeof...(_Tp), sizeof...(_Up)>::type,
                    typename __make_tuple_types<tuple, sizeof...(_Tp), sizeof...(_Up)>::type,
                    _Up...
                >::value
            ))
            : __base_(typename __make_tuple_indices<sizeof...(_Up)>::type(),
                    typename __make_tuple_types<tuple, sizeof...(_Up)>::type(),
                    typename __make_tuple_indices<sizeof...(_Tp), sizeof...(_Up)>::type(),
                    typename __make_tuple_types<tuple, sizeof...(_Tp), sizeof...(_Up)>::type(),
                    _VSTD::forward<_Up>(__u)...) {}

    template <class ..._Up,
              typename enable_if
                      <
                         _CheckArgsConstructor<
                             sizeof...(_Up) <= sizeof...(_Tp)
                             && !_PackExpandsToThisTuple<_Up...>::value
                         >::template __enable_explicit<_Up...>() ||
                         _CheckArgsConstructor<
                            !_EnableImplicitReducedArityExtension
                            && sizeof...(_Up) < sizeof...(_Tp)
                            && !_PackExpandsToThisTuple<_Up...>::value
                         >::template __enable_implicit<_Up...>(),
                         bool
                      >::type = false
             >
        _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
        explicit
        tuple(_Up&&... __u)
            _NOEXCEPT_((
                is_nothrow_constructible<_BaseT,
                    typename __make_tuple_indices<sizeof...(_Up)>::type,
                    typename __make_tuple_types<tuple, sizeof...(_Up)>::type,
                    typename __make_tuple_indices<sizeof...(_Tp), sizeof...(_Up)>::type,
                    typename __make_tuple_types<tuple, sizeof...(_Tp), sizeof...(_Up)>::type,
                    _Up...
                >::value
            ))
            : __base_(typename __make_tuple_indices<sizeof...(_Up)>::type(),
                    typename __make_tuple_types<tuple, sizeof...(_Up)>::type(),
                    typename __make_tuple_indices<sizeof...(_Tp), sizeof...(_Up)>::type(),
                    typename __make_tuple_types<tuple, sizeof...(_Tp), sizeof...(_Up)>::type(),
                    _VSTD::forward<_Up>(__u)...) {}

    template <class _Alloc, class ..._Up,
              typename enable_if
                      <
                         _CheckArgsConstructor<
                             sizeof...(_Up) == sizeof...(_Tp) &&
                             !_PackExpandsToThisTuple<_Up...>::value
                         >::template __enable_implicit<_Up...>(),
                         bool
                      >::type = false
             >
        _LIBCPP_INLINE_VISIBILITY
        tuple(allocator_arg_t, const _Alloc& __a, _Up&&... __u)
            : __base_(allocator_arg_t(), __a,
                    typename __make_tuple_indices<sizeof...(_Up)>::type(),
                    typename __make_tuple_types<tuple, sizeof...(_Up)>::type(),
                    typename __make_tuple_indices<sizeof...(_Tp), sizeof...(_Up)>::type(),
                    typename __make_tuple_types<tuple, sizeof...(_Tp), sizeof...(_Up)>::type(),
                    _VSTD::forward<_Up>(__u)...) {}

    template <class _Alloc, class ..._Up,
              typename enable_if
                      <
                         _CheckArgsConstructor<
                             sizeof...(_Up) == sizeof...(_Tp) &&
                             !_PackExpandsToThisTuple<_Up...>::value
                         >::template __enable_explicit<_Up...>(),
                         bool
                      >::type = false
             >
        _LIBCPP_INLINE_VISIBILITY
        explicit
        tuple(allocator_arg_t, const _Alloc& __a, _Up&&... __u)
            : __base_(allocator_arg_t(), __a,
                    typename __make_tuple_indices<sizeof...(_Up)>::type(),
                    typename __make_tuple_types<tuple, sizeof...(_Up)>::type(),
                    typename __make_tuple_indices<sizeof...(_Tp), sizeof...(_Up)>::type(),
                    typename __make_tuple_types<tuple, sizeof...(_Tp), sizeof...(_Up)>::type(),
                    _VSTD::forward<_Up>(__u)...) {}

    template <class _Tuple,
              typename enable_if
                      <
                         _CheckTupleLikeConstructor<
                             __tuple_like_with_size<_Tuple, sizeof...(_Tp)>::value
                             && !_PackExpandsToThisTuple<_Tuple>::value
                         >::template __enable_implicit<_Tuple>(),
                         bool
                      >::type = false
             >
        _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
        tuple(_Tuple&& __t) _NOEXCEPT_((is_nothrow_constructible<_BaseT, _Tuple>::value))
            : __base_(_VSTD::forward<_Tuple>(__t)) {}

    template <class _Tuple,
              typename enable_if
                      <
                         _CheckTupleLikeConstructor<
                             __tuple_like_with_size<_Tuple, sizeof...(_Tp)>::value
                             && !_PackExpandsToThisTuple<_Tuple>::value
                         >::template __enable_explicit<_Tuple>(),
                         bool
                      >::type = false
             >
        _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
        explicit
        tuple(_Tuple&& __t) _NOEXCEPT_((is_nothrow_constructible<_BaseT, _Tuple>::value))
            : __base_(_VSTD::forward<_Tuple>(__t)) {}

    template <class _Alloc, class _Tuple,
              typename enable_if
                      <
                         _CheckTupleLikeConstructor<
                             __tuple_like_with_size<_Tuple, sizeof...(_Tp)>::value
                         >::template __enable_implicit<_Tuple>(),
                         bool
                      >::type = false
             >
        _LIBCPP_INLINE_VISIBILITY
        tuple(allocator_arg_t, const _Alloc& __a, _Tuple&& __t)
            : __base_(allocator_arg_t(), __a, _VSTD::forward<_Tuple>(__t)) {}

    template <class _Alloc, class _Tuple,
              typename enable_if
                      <
                         _CheckTupleLikeConstructor<
                             __tuple_like_with_size<_Tuple, sizeof...(_Tp)>::value
                         >::template __enable_explicit<_Tuple>(),
                         bool
                      >::type = false
             >
        _LIBCPP_INLINE_VISIBILITY
        explicit
        tuple(allocator_arg_t, const _Alloc& __a, _Tuple&& __t)
            : __base_(allocator_arg_t(), __a, _VSTD::forward<_Tuple>(__t)) {}

    using _CanCopyAssign = __all<is_copy_assignable<_Tp>::value...>;
    using _CanMoveAssign = __all<is_move_assignable<_Tp>::value...>;

    _LIBCPP_INLINE_VISIBILITY
    tuple& operator=(typename conditional<_CanCopyAssign::value, tuple, __nat>::type const& __t)
        _NOEXCEPT_((__all<is_nothrow_copy_assignable<_Tp>::value...>::value))
    {
        __base_.operator=(__t.__base_);
        return *this;
    }

    _LIBCPP_INLINE_VISIBILITY
    tuple& operator=(typename conditional<_CanMoveAssign::value, tuple, __nat>::type&& __t)
        _NOEXCEPT_((__all<is_nothrow_move_assignable<_Tp>::value...>::value))
    {
        __base_.operator=(static_cast<_BaseT&&>(__t.__base_));
        return *this;
    }

    template <class _Tuple,
              class = typename enable_if
                      <
                         __tuple_assignable<_Tuple, tuple>::value
                      >::type
             >
        _LIBCPP_INLINE_VISIBILITY
        tuple&
        operator=(_Tuple&& __t) _NOEXCEPT_((is_nothrow_assignable<_BaseT&, _Tuple>::value))
        {
            __base_.operator=(_VSTD::forward<_Tuple>(__t));
            return *this;
        }

    _LIBCPP_INLINE_VISIBILITY
    void swap(tuple& __t) _NOEXCEPT_(__all<__is_nothrow_swappable<_Tp>::value...>::value)
        {__base_.swap(__t.__base_);}
};

template <>
class _LIBCPP_TEMPLATE_VIS tuple<>
{
public:
    _LIBCPP_INLINE_VISIBILITY
    _LIBCPP_CONSTEXPR tuple() _NOEXCEPT {}
    template <class _Alloc>
    _LIBCPP_INLINE_VISIBILITY
        tuple(allocator_arg_t, const _Alloc&) _NOEXCEPT {}
    template <class _Alloc>
    _LIBCPP_INLINE_VISIBILITY
        tuple(allocator_arg_t, const _Alloc&, const tuple&) _NOEXCEPT {}
    template <class _Up>
    _LIBCPP_INLINE_VISIBILITY
    void swap(tuple&) _NOEXCEPT {}
};

#ifdef __cpp_deduction_guides
// NOTE: These are not yet standardized, but are required to simulate the
// implicit deduction guide that should be generated had libc++ declared the
// tuple-like constructors "correctly"
template <class _Alloc, class ..._Args>
tuple(allocator_arg_t, const _Alloc&, tuple<_Args...> const&) -> tuple<_Args...>;
template <class _Alloc, class ..._Args>
tuple(allocator_arg_t, const _Alloc&, tuple<_Args...>&&) -> tuple<_Args...>;
#endif

template <class ..._Tp>
inline _LIBCPP_INLINE_VISIBILITY
typename enable_if
<
    __all<__is_swappable<_Tp>::value...>::value,
    void
>::type
swap(tuple<_Tp...>& __t, tuple<_Tp...>& __u)
                 _NOEXCEPT_(__all<__is_nothrow_swappable<_Tp>::value...>::value)
    {__t.swap(__u);}

// get

template <size_t _Ip, class ..._Tp>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
typename tuple_element<_Ip, tuple<_Tp...> >::type&
get(tuple<_Tp...>& __t) _NOEXCEPT
{
    typedef typename tuple_element<_Ip, tuple<_Tp...> >::type type;
    return static_cast<__tuple_leaf<_Ip, type>&>(__t.__base_).get();
}

template <size_t _Ip, class ..._Tp>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
const typename tuple_element<_Ip, tuple<_Tp...> >::type&
get(const tuple<_Tp...>& __t) _NOEXCEPT
{
    typedef typename tuple_element<_Ip, tuple<_Tp...> >::type type;
    return static_cast<const __tuple_leaf<_Ip, type>&>(__t.__base_).get();
}

template <size_t _Ip, class ..._Tp>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
typename tuple_element<_Ip, tuple<_Tp...> >::type&&
get(tuple<_Tp...>&& __t) _NOEXCEPT
{
    typedef typename tuple_element<_Ip, tuple<_Tp...> >::type type;
    return static_cast<type&&>(
             static_cast<__tuple_leaf<_Ip, type>&&>(__t.__base_).get());
}

template <size_t _Ip, class ..._Tp>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
const typename tuple_element<_Ip, tuple<_Tp...> >::type&&
get(const tuple<_Tp...>&& __t) _NOEXCEPT
{
    typedef typename tuple_element<_Ip, tuple<_Tp...> >::type type;
    return static_cast<const type&&>(
             static_cast<const __tuple_leaf<_Ip, type>&&>(__t.__base_).get());
}

#if _LIBCPP_STD_VER > 11

namespace __find_detail {

static constexpr size_t __not_found = -1;
static constexpr size_t __ambiguous = __not_found - 1;

inline _LIBCPP_INLINE_VISIBILITY
constexpr size_t __find_idx_return(size_t __curr_i, size_t __res, bool __matches) {
    return !__matches ? __res :
        (__res == __not_found ? __curr_i : __ambiguous);
}

template <size_t _Nx>
inline _LIBCPP_INLINE_VISIBILITY
constexpr size_t __find_idx(size_t __i, const bool (&__matches)[_Nx]) {
  return __i == _Nx ? __not_found :
      __find_idx_return(__i, __find_idx(__i + 1, __matches), __matches[__i]);
}

template <class _T1, class ..._Args>
struct __find_exactly_one_checked {
  static constexpr bool __matches[] = {is_same<_T1, _Args>::value...};
    static constexpr size_t value = __find_detail::__find_idx(0, __matches);
    static_assert (value != __not_found, "type not found in type list" );
    static_assert(value != __ambiguous,"type occurs more than once in type list");
};

template <class _T1>
struct __find_exactly_one_checked<_T1> {
    static_assert(!is_same<_T1, _T1>::value, "type not in empty type list");
};

} // namespace __find_detail;

template <typename _T1, typename... _Args>
struct __find_exactly_one_t
    : public __find_detail::__find_exactly_one_checked<_T1, _Args...> {
};

template <class _T1, class... _Args>
inline _LIBCPP_INLINE_VISIBILITY
constexpr _T1& get(tuple<_Args...>& __tup) noexcept
{
    return _VSTD::get<__find_exactly_one_t<_T1, _Args...>::value>(__tup);
}

template <class _T1, class... _Args>
inline _LIBCPP_INLINE_VISIBILITY
constexpr _T1 const& get(tuple<_Args...> const& __tup) noexcept
{
    return _VSTD::get<__find_exactly_one_t<_T1, _Args...>::value>(__tup);
}

template <class _T1, class... _Args>
inline _LIBCPP_INLINE_VISIBILITY
constexpr _T1&& get(tuple<_Args...>&& __tup) noexcept
{
    return _VSTD::get<__find_exactly_one_t<_T1, _Args...>::value>(_VSTD::move(__tup));
}

template <class _T1, class... _Args>
inline _LIBCPP_INLINE_VISIBILITY
constexpr _T1 const&& get(tuple<_Args...> const&& __tup) noexcept
{
    return _VSTD::get<__find_exactly_one_t<_T1, _Args...>::value>(_VSTD::move(__tup));
}

#endif

// tie

template <class ..._Tp>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
tuple<_Tp&...>
tie(_Tp&... __t) _NOEXCEPT
{
    return tuple<_Tp&...>(__t...);
}

template <class _Up>
struct __ignore_t
{
    template <class _Tp>
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
    const __ignore_t& operator=(_Tp&&) const {return *this;}
};

namespace {
  constexpr __ignore_t<unsigned char> ignore = __ignore_t<unsigned char>();
}

template <class _Tp>
struct __make_tuple_return_impl
{
    typedef _Tp type;
};

template <class _Tp>
struct __make_tuple_return_impl<reference_wrapper<_Tp> >
{
    typedef _Tp& type;
};

template <class _Tp>
struct __make_tuple_return
{
    typedef typename __make_tuple_return_impl<typename decay<_Tp>::type>::type type;
};

template <class... _Tp>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
tuple<typename __make_tuple_return<_Tp>::type...>
make_tuple(_Tp&&... __t)
{
    return tuple<typename __make_tuple_return<_Tp>::type...>(_VSTD::forward<_Tp>(__t)...);
}

template <class... _Tp>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
tuple<_Tp&&...>
forward_as_tuple(_Tp&&... __t) _NOEXCEPT
{
    return tuple<_Tp&&...>(_VSTD::forward<_Tp>(__t)...);
}

template <size_t _Ip>
struct __tuple_equal
{
    template <class _Tp, class _Up>
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
    bool operator()(const _Tp& __x, const _Up& __y)
    {
        return __tuple_equal<_Ip - 1>()(__x, __y) && _VSTD::get<_Ip-1>(__x) == _VSTD::get<_Ip-1>(__y);
    }
};

template <>
struct __tuple_equal<0>
{
    template <class _Tp, class _Up>
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
    bool operator()(const _Tp&, const _Up&)
    {
        return true;
    }
};

template <class ..._Tp, class ..._Up>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
bool
operator==(const tuple<_Tp...>& __x, const tuple<_Up...>& __y)
{
    return __tuple_equal<sizeof...(_Tp)>()(__x, __y);
}

template <class ..._Tp, class ..._Up>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
bool
operator!=(const tuple<_Tp...>& __x, const tuple<_Up...>& __y)
{
    return !(__x == __y);
}

template <size_t _Ip>
struct __tuple_less
{
    template <class _Tp, class _Up>
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
    bool operator()(const _Tp& __x, const _Up& __y)
    {
        const size_t __idx = tuple_size<_Tp>::value - _Ip;
        if (_VSTD::get<__idx>(__x) < _VSTD::get<__idx>(__y))
            return true;
        if (_VSTD::get<__idx>(__y) < _VSTD::get<__idx>(__x))
            return false;
        return __tuple_less<_Ip-1>()(__x, __y);
    }
};

template <>
struct __tuple_less<0>
{
    template <class _Tp, class _Up>
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
    bool operator()(const _Tp&, const _Up&)
    {
        return false;
    }
};

template <class ..._Tp, class ..._Up>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
bool
operator<(const tuple<_Tp...>& __x, const tuple<_Up...>& __y)
{
    return __tuple_less<sizeof...(_Tp)>()(__x, __y);
}

template <class ..._Tp, class ..._Up>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
bool
operator>(const tuple<_Tp...>& __x, const tuple<_Up...>& __y)
{
    return __y < __x;
}

template <class ..._Tp, class ..._Up>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
bool
operator>=(const tuple<_Tp...>& __x, const tuple<_Up...>& __y)
{
    return !(__x < __y);
}

template <class ..._Tp, class ..._Up>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
bool
operator<=(const tuple<_Tp...>& __x, const tuple<_Up...>& __y)
{
    return !(__y < __x);
}

// tuple_cat

template <class _Tp, class _Up> struct __tuple_cat_type;

template <class ..._Ttypes, class ..._Utypes>
struct __tuple_cat_type<tuple<_Ttypes...>, __tuple_types<_Utypes...> >
{
    typedef tuple<_Ttypes..., _Utypes...> type;
};

template <class _ResultTuple, bool _Is_Tuple0TupleLike, class ..._Tuples>
struct __tuple_cat_return_1
{
};

template <class ..._Types, class _Tuple0>
struct __tuple_cat_return_1<tuple<_Types...>, true, _Tuple0>
{
    typedef typename __tuple_cat_type<tuple<_Types...>,
            typename __make_tuple_types<typename remove_reference<_Tuple0>::type>::type>::type
                                                                           type;
};

template <class ..._Types, class _Tuple0, class _Tuple1, class ..._Tuples>
struct __tuple_cat_return_1<tuple<_Types...>, true, _Tuple0, _Tuple1, _Tuples...>
    : public __tuple_cat_return_1<
                 typename __tuple_cat_type<
                     tuple<_Types...>,
                     typename __make_tuple_types<typename remove_reference<_Tuple0>::type>::type
                 >::type,
                 __tuple_like<typename remove_reference<_Tuple1>::type>::value,
                 _Tuple1, _Tuples...>
{
};

template <class ..._Tuples> struct __tuple_cat_return;

template <class _Tuple0, class ..._Tuples>
struct __tuple_cat_return<_Tuple0, _Tuples...>
    : public __tuple_cat_return_1<tuple<>,
         __tuple_like<typename remove_reference<_Tuple0>::type>::value, _Tuple0,
                                                                     _Tuples...>
{
};

template <>
struct __tuple_cat_return<>
{
    typedef tuple<> type;
};

inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
tuple<>
tuple_cat()
{
    return tuple<>();
}

template <class _Rp, class _Indices, class _Tuple0, class ..._Tuples>
struct __tuple_cat_return_ref_imp;

template <class ..._Types, size_t ..._I0, class _Tuple0>
struct __tuple_cat_return_ref_imp<tuple<_Types...>, __tuple_indices<_I0...>, _Tuple0>
{
    typedef typename remove_reference<_Tuple0>::type _T0;
    typedef tuple<_Types..., typename __apply_cv<_Tuple0,
                          typename tuple_element<_I0, _T0>::type>::type&&...> type;
};

template <class ..._Types, size_t ..._I0, class _Tuple0, class _Tuple1, class ..._Tuples>
struct __tuple_cat_return_ref_imp<tuple<_Types...>, __tuple_indices<_I0...>,
                                  _Tuple0, _Tuple1, _Tuples...>
    : public __tuple_cat_return_ref_imp<
         tuple<_Types..., typename __apply_cv<_Tuple0,
               typename tuple_element<_I0,
                  typename remove_reference<_Tuple0>::type>::type>::type&&...>,
         typename __make_tuple_indices<tuple_size<typename
                                 remove_reference<_Tuple1>::type>::value>::type,
         _Tuple1, _Tuples...>
{
};

template <class _Tuple0, class ..._Tuples>
struct __tuple_cat_return_ref
    : public __tuple_cat_return_ref_imp<tuple<>,
               typename __make_tuple_indices<
                        tuple_size<typename remove_reference<_Tuple0>::type>::value
               >::type, _Tuple0, _Tuples...>
{
};

template <class _Types, class _I0, class _J0>
struct __tuple_cat;

template <class ..._Types, size_t ..._I0, size_t ..._J0>
struct __tuple_cat<tuple<_Types...>, __tuple_indices<_I0...>, __tuple_indices<_J0...> >
{
    template <class _Tuple0>
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
    typename __tuple_cat_return_ref<tuple<_Types...>&&, _Tuple0&&>::type
    operator()(tuple<_Types...> __t, _Tuple0&& __t0)
    {
        return forward_as_tuple(_VSTD::forward<_Types>(_VSTD::get<_I0>(__t))...,
                                      _VSTD::get<_J0>(_VSTD::forward<_Tuple0>(__t0))...);
    }

    template <class _Tuple0, class _Tuple1, class ..._Tuples>
    _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
    typename __tuple_cat_return_ref<tuple<_Types...>&&, _Tuple0&&, _Tuple1&&, _Tuples&&...>::type
    operator()(tuple<_Types...> __t, _Tuple0&& __t0, _Tuple1&& __t1, _Tuples&& ...__tpls)
    {
        typedef typename remove_reference<_Tuple0>::type _T0;
        typedef typename remove_reference<_Tuple1>::type _T1;
        return __tuple_cat<
           tuple<_Types..., typename __apply_cv<_Tuple0, typename tuple_element<_J0, _T0>::type>::type&&...>,
           typename __make_tuple_indices<sizeof ...(_Types) + tuple_size<_T0>::value>::type,
           typename __make_tuple_indices<tuple_size<_T1>::value>::type>()
                           (forward_as_tuple(
                              _VSTD::forward<_Types>(_VSTD::get<_I0>(__t))...,
                              _VSTD::get<_J0>(_VSTD::forward<_Tuple0>(__t0))...
                            ),
                            _VSTD::forward<_Tuple1>(__t1),
                            _VSTD::forward<_Tuples>(__tpls)...);
    }
};

template <class _Tuple0, class... _Tuples>
inline _LIBCPP_INLINE_VISIBILITY _LIBCPP_CONSTEXPR_AFTER_CXX11
typename __tuple_cat_return<_Tuple0, _Tuples...>::type
tuple_cat(_Tuple0&& __t0, _Tuples&&... __tpls)
{
    typedef typename remove_reference<_Tuple0>::type _T0;
    return __tuple_cat<tuple<>, __tuple_indices<>,
                  typename __make_tuple_indices<tuple_size<_T0>::value>::type>()
                  (tuple<>(), _VSTD::forward<_Tuple0>(__t0),
                                            _VSTD::forward<_Tuples>(__tpls)...);
}

template <class ..._Tp, class _Alloc>
struct _LIBCPP_TEMPLATE_VIS uses_allocator<tuple<_Tp...>, _Alloc>
    : true_type {};

template <class _T1, class _T2>
template <class... _Args1, class... _Args2, size_t ..._I1, size_t ..._I2>
inline _LIBCPP_INLINE_VISIBILITY
pair<_T1, _T2>::pair(piecewise_construct_t,
                     tuple<_Args1...>& __first_args, tuple<_Args2...>& __second_args,
                     __tuple_indices<_I1...>, __tuple_indices<_I2...>)
    :  first(_VSTD::forward<_Args1>(_VSTD::get<_I1>( __first_args))...),
      second(_VSTD::forward<_Args2>(_VSTD::get<_I2>(__second_args))...)
{
}

#if _LIBCPP_STD_VER > 14
template <class _Tp>
constexpr size_t tuple_size_v = tuple_size<_Tp>::value;

#define _LIBCPP_NOEXCEPT_RETURN(...) noexcept(noexcept(__VA_ARGS__)) { return __VA_ARGS__; }

template <class _Fn, class _Tuple, size_t ..._Id>
inline _LIBCPP_INLINE_VISIBILITY
constexpr decltype(auto) __apply_tuple_impl(_Fn && __f, _Tuple && __t,
                                            __tuple_indices<_Id...>)
_LIBCPP_NOEXCEPT_RETURN(
    _VSTD::__invoke_constexpr(
        _VSTD::forward<_Fn>(__f),
        _VSTD::get<_Id>(_VSTD::forward<_Tuple>(__t))...)
)

template <class _Fn, class _Tuple>
inline _LIBCPP_INLINE_VISIBILITY
constexpr decltype(auto) apply(_Fn && __f, _Tuple && __t)
_LIBCPP_NOEXCEPT_RETURN(
    _VSTD::__apply_tuple_impl(
        _VSTD::forward<_Fn>(__f), _VSTD::forward<_Tuple>(__t),
        typename __make_tuple_indices<tuple_size_v<decay_t<_Tuple>>>::type{})
)

template <class _Tp, class _Tuple, size_t... _Idx>
inline _LIBCPP_INLINE_VISIBILITY
constexpr _Tp __make_from_tuple_impl(_Tuple&& __t, __tuple_indices<_Idx...>)
_LIBCPP_NOEXCEPT_RETURN(
    _Tp(_VSTD::get<_Idx>(_VSTD::forward<_Tuple>(__t))...)
)

template <class _Tp, class _Tuple>
inline _LIBCPP_INLINE_VISIBILITY
constexpr _Tp make_from_tuple(_Tuple&& __t)
_LIBCPP_NOEXCEPT_RETURN(
    _VSTD::__make_from_tuple_impl<_Tp>(_VSTD::forward<_Tuple>(__t),
        typename __make_tuple_indices<tuple_size_v<decay_t<_Tuple>>>::type{})
)

#undef _LIBCPP_NOEXCEPT_RETURN

#endif // _LIBCPP_STD_VER > 14

#endif // !defined(_LIBCPP_CXX03_LANG)

_LIBCPP_END_NAMESPACE_STD

#endif  // _LIBCPP_TUPLE
