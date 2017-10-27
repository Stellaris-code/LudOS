// -*- C++ -*-
//===---------------------------- set -------------------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is dual licensed under the MIT and the University of Illinois Open
// Source Licenses. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#ifndef _LIBCPP_SET
#define _LIBCPP_SET

#include <__config.hpp>
#include <__tree.hpp>
#include <functional.hpp>

#if !defined(_LIBCPP_HAS_NO_PRAGMA_SYSTEM_HEADER)
#pragma GCC system_header
#endif

_LIBCPP_BEGIN_NAMESPACE_STD

template <class _Key, class _Compare = less<_Key>,
          class _Allocator = allocator<_Key> >
class _LIBCPP_TEMPLATE_VIS set
{
public:
    // types:
    typedef _Key                                     key_type;
    typedef key_type                                 value_type;
    typedef _Compare                                 key_compare;
    typedef key_compare                              value_compare;
    typedef _Allocator                               allocator_type;
    typedef value_type&                              reference;
    typedef const value_type&                        const_reference;

    static_assert((is_same<typename allocator_type::value_type, value_type>::value),
                  "Allocator::value_type must be same type as value_type");

private:
    typedef __tree<value_type, value_compare, allocator_type> __base;
    typedef allocator_traits<allocator_type>                  __alloc_traits;
    typedef typename __base::__node_holder                    __node_holder;

    __base __tree_;

public:
    typedef typename __base::pointer               pointer;
    typedef typename __base::const_pointer         const_pointer;
    typedef typename __base::size_type             size_type;
    typedef typename __base::difference_type       difference_type;
    typedef typename __base::const_iterator        iterator;
    typedef typename __base::const_iterator        const_iterator;
    typedef _VSTD::reverse_iterator<iterator>       reverse_iterator;
    typedef _VSTD::reverse_iterator<const_iterator> const_reverse_iterator;

    _LIBCPP_INLINE_VISIBILITY
    set()
        _NOEXCEPT_(
            is_nothrow_default_constructible<allocator_type>::value &&
            is_nothrow_default_constructible<key_compare>::value &&
            is_nothrow_copy_constructible<key_compare>::value)
        : __tree_(value_compare()) {}

    _LIBCPP_INLINE_VISIBILITY
    explicit set(const value_compare& __comp)
        _NOEXCEPT_(
            is_nothrow_default_constructible<allocator_type>::value &&
            is_nothrow_copy_constructible<key_compare>::value)
        : __tree_(__comp) {}

    _LIBCPP_INLINE_VISIBILITY
    explicit set(const value_compare& __comp, const allocator_type& __a)
        : __tree_(__comp, __a) {}
    template <class _InputIterator>
        _LIBCPP_INLINE_VISIBILITY
        set(_InputIterator __f, _InputIterator __l,
            const value_compare& __comp = value_compare())
        : __tree_(__comp)
        {
            insert(__f, __l);
        }

    template <class _InputIterator>
        _LIBCPP_INLINE_VISIBILITY
        set(_InputIterator __f, _InputIterator __l, const value_compare& __comp,
            const allocator_type& __a)
        : __tree_(__comp, __a)
        {
            insert(__f, __l);
        }

#if _LIBCPP_STD_VER > 11
        template <class _InputIterator>
        _LIBCPP_INLINE_VISIBILITY
        set(_InputIterator __f, _InputIterator __l, const allocator_type& __a)
            : set(__f, __l, key_compare(), __a) {}
#endif

    _LIBCPP_INLINE_VISIBILITY
    set(const set& __s)
        : __tree_(__s.__tree_)
        {
            insert(__s.begin(), __s.end());
        }

    _LIBCPP_INLINE_VISIBILITY
    set& operator=(const set& __s)
        {
            __tree_ = __s.__tree_;
            return *this;
        }

#ifndef _LIBCPP_CXX03_LANG
    _LIBCPP_INLINE_VISIBILITY
    set(set&& __s)
        _NOEXCEPT_(is_nothrow_move_constructible<__base>::value)
        : __tree_(_VSTD::move(__s.__tree_)) {}
#endif  // _LIBCPP_CXX03_LANG

    _LIBCPP_INLINE_VISIBILITY
    explicit set(const allocator_type& __a)
        : __tree_(__a) {}

    _LIBCPP_INLINE_VISIBILITY
    set(const set& __s, const allocator_type& __a)
        : __tree_(__s.__tree_.value_comp(), __a)
        {
            insert(__s.begin(), __s.end());
        }

#ifndef _LIBCPP_CXX03_LANG
    set(set&& __s, const allocator_type& __a);

    _LIBCPP_INLINE_VISIBILITY
    set(initializer_list<value_type> __il, const value_compare& __comp = value_compare())
        : __tree_(__comp)
        {
            insert(__il.begin(), __il.end());
        }

    _LIBCPP_INLINE_VISIBILITY
    set(initializer_list<value_type> __il, const value_compare& __comp,
        const allocator_type& __a)
        : __tree_(__comp, __a)
        {
            insert(__il.begin(), __il.end());
        }

#if _LIBCPP_STD_VER > 11
    _LIBCPP_INLINE_VISIBILITY
    set(initializer_list<value_type> __il, const allocator_type& __a)
        : set(__il, key_compare(), __a) {}
#endif

    _LIBCPP_INLINE_VISIBILITY
    set& operator=(initializer_list<value_type> __il)
        {
            __tree_.__assign_unique(__il.begin(), __il.end());
            return *this;
        }

    _LIBCPP_INLINE_VISIBILITY
    set& operator=(set&& __s)
        _NOEXCEPT_(is_nothrow_move_assignable<__base>::value)
        {
            __tree_ = _VSTD::move(__s.__tree_);
            return *this;
        }
#endif  // _LIBCPP_CXX03_LANG

    _LIBCPP_INLINE_VISIBILITY
          iterator begin() _NOEXCEPT       {return __tree_.begin();}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator begin() const _NOEXCEPT {return __tree_.begin();}
    _LIBCPP_INLINE_VISIBILITY
          iterator end() _NOEXCEPT         {return __tree_.end();}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator end()   const _NOEXCEPT {return __tree_.end();}

    _LIBCPP_INLINE_VISIBILITY
          reverse_iterator rbegin() _NOEXCEPT
            {return reverse_iterator(end());}
    _LIBCPP_INLINE_VISIBILITY
    const_reverse_iterator rbegin() const _NOEXCEPT
        {return const_reverse_iterator(end());}
    _LIBCPP_INLINE_VISIBILITY
          reverse_iterator rend() _NOEXCEPT
            {return reverse_iterator(begin());}
    _LIBCPP_INLINE_VISIBILITY
    const_reverse_iterator rend() const _NOEXCEPT
        {return const_reverse_iterator(begin());}

    _LIBCPP_INLINE_VISIBILITY
    const_iterator cbegin()  const _NOEXCEPT {return begin();}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator cend() const _NOEXCEPT {return end();}
    _LIBCPP_INLINE_VISIBILITY
    const_reverse_iterator crbegin() const _NOEXCEPT {return rbegin();}
    _LIBCPP_INLINE_VISIBILITY
    const_reverse_iterator crend() const _NOEXCEPT {return rend();}

    _LIBCPP_INLINE_VISIBILITY
    bool empty() const _NOEXCEPT {return __tree_.size() == 0;}
    _LIBCPP_INLINE_VISIBILITY
    size_type size() const _NOEXCEPT {return __tree_.size();}
    _LIBCPP_INLINE_VISIBILITY
    size_type max_size() const _NOEXCEPT {return __tree_.max_size();}

    // modifiers:
#ifndef _LIBCPP_CXX03_LANG
    template <class... _Args>
        _LIBCPP_INLINE_VISIBILITY
        pair<iterator, bool> emplace(_Args&&... __args)
            {return __tree_.__emplace_unique(_VSTD::forward<_Args>(__args)...);}
    template <class... _Args>
        _LIBCPP_INLINE_VISIBILITY
        iterator emplace_hint(const_iterator __p, _Args&&... __args)
            {return __tree_.__emplace_hint_unique(__p, _VSTD::forward<_Args>(__args)...);}
#endif  // _LIBCPP_CXX03_LANG

    _LIBCPP_INLINE_VISIBILITY
    pair<iterator,bool> insert(const value_type& __v)
        {return __tree_.__insert_unique(__v);}
    _LIBCPP_INLINE_VISIBILITY
    iterator insert(const_iterator __p, const value_type& __v)
        {return __tree_.__insert_unique(__p, __v);}

    template <class _InputIterator>
        _LIBCPP_INLINE_VISIBILITY
        void insert(_InputIterator __f, _InputIterator __l)
        {
            for (const_iterator __e = cend(); __f != __l; ++__f)
                __tree_.__insert_unique(__e, *__f);
        }

#ifndef _LIBCPP_CXX03_LANG
    _LIBCPP_INLINE_VISIBILITY
    pair<iterator,bool> insert(value_type&& __v)
        {return __tree_.__insert_unique(_VSTD::move(__v));}

    _LIBCPP_INLINE_VISIBILITY
    iterator insert(const_iterator __p, value_type&& __v)
        {return __tree_.__insert_unique(__p, _VSTD::move(__v));}

    _LIBCPP_INLINE_VISIBILITY
    void insert(initializer_list<value_type> __il)
        {insert(__il.begin(), __il.end());}
#endif  // _LIBCPP_CXX03_LANG

    _LIBCPP_INLINE_VISIBILITY
    iterator  erase(const_iterator __p) {return __tree_.erase(__p);}
    _LIBCPP_INLINE_VISIBILITY
    size_type erase(const key_type& __k)
        {return __tree_.__erase_unique(__k);}
    _LIBCPP_INLINE_VISIBILITY
    iterator  erase(const_iterator __f, const_iterator __l)
        {return __tree_.erase(__f, __l);}
    _LIBCPP_INLINE_VISIBILITY
    void clear() _NOEXCEPT {__tree_.clear();}

    _LIBCPP_INLINE_VISIBILITY
    void swap(set& __s) _NOEXCEPT_(__is_nothrow_swappable<__base>::value)
        {__tree_.swap(__s.__tree_);}

    _LIBCPP_INLINE_VISIBILITY
    allocator_type get_allocator() const _NOEXCEPT {return __tree_.__alloc();}
    _LIBCPP_INLINE_VISIBILITY
    key_compare    key_comp()      const {return __tree_.value_comp();}
    _LIBCPP_INLINE_VISIBILITY
    value_compare  value_comp()    const {return __tree_.value_comp();}

    // set operations:
    _LIBCPP_INLINE_VISIBILITY
    iterator find(const key_type& __k)             {return __tree_.find(__k);}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator find(const key_type& __k) const {return __tree_.find(__k);}
#if _LIBCPP_STD_VER > 11
    template <typename _K2>
    _LIBCPP_INLINE_VISIBILITY
    typename enable_if<__is_transparent<_Compare, _K2>::value,iterator>::type
    find(const _K2& __k)                           {return __tree_.find(__k);}
    template <typename _K2>
    _LIBCPP_INLINE_VISIBILITY
    typename enable_if<__is_transparent<_Compare, _K2>::value,const_iterator>::type
    find(const _K2& __k) const                     {return __tree_.find(__k);}
#endif

    _LIBCPP_INLINE_VISIBILITY
    size_type      count(const key_type& __k) const
        {return __tree_.__count_unique(__k);}
#if _LIBCPP_STD_VER > 11
    template <typename _K2>
    _LIBCPP_INLINE_VISIBILITY
    typename enable_if<__is_transparent<_Compare, _K2>::value,size_type>::type
    count(const _K2& __k) const                    {return __tree_.__count_unique(__k);}
#endif
    _LIBCPP_INLINE_VISIBILITY
    iterator lower_bound(const key_type& __k)
        {return __tree_.lower_bound(__k);}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator lower_bound(const key_type& __k) const
        {return __tree_.lower_bound(__k);}
#if _LIBCPP_STD_VER > 11
    template <typename _K2>
    _LIBCPP_INLINE_VISIBILITY
    typename enable_if<__is_transparent<_Compare, _K2>::value,iterator>::type
    lower_bound(const _K2& __k)       {return __tree_.lower_bound(__k);}

    template <typename _K2>
    _LIBCPP_INLINE_VISIBILITY
    typename enable_if<__is_transparent<_Compare, _K2>::value,const_iterator>::type
    lower_bound(const _K2& __k) const {return __tree_.lower_bound(__k);}
#endif

    _LIBCPP_INLINE_VISIBILITY
    iterator upper_bound(const key_type& __k)
        {return __tree_.upper_bound(__k);}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator upper_bound(const key_type& __k) const
        {return __tree_.upper_bound(__k);}
#if _LIBCPP_STD_VER > 11
    template <typename _K2>
    _LIBCPP_INLINE_VISIBILITY
    typename enable_if<__is_transparent<_Compare, _K2>::value,iterator>::type
    upper_bound(const _K2& __k)       {return __tree_.upper_bound(__k);}
    template <typename _K2>
    _LIBCPP_INLINE_VISIBILITY
    typename enable_if<__is_transparent<_Compare, _K2>::value,const_iterator>::type
    upper_bound(const _K2& __k) const {return __tree_.upper_bound(__k);}
#endif

    _LIBCPP_INLINE_VISIBILITY
    pair<iterator,iterator> equal_range(const key_type& __k)
        {return __tree_.__equal_range_unique(__k);}
    _LIBCPP_INLINE_VISIBILITY
    pair<const_iterator,const_iterator> equal_range(const key_type& __k) const
        {return __tree_.__equal_range_unique(__k);}
#if _LIBCPP_STD_VER > 11
    template <typename _K2>
    _LIBCPP_INLINE_VISIBILITY
    typename enable_if<__is_transparent<_Compare, _K2>::value,pair<iterator,iterator>>::type
    equal_range(const _K2& __k)       {return __tree_.__equal_range_unique(__k);}
    template <typename _K2>
    _LIBCPP_INLINE_VISIBILITY
    typename enable_if<__is_transparent<_Compare, _K2>::value,pair<const_iterator,const_iterator>>::type
    equal_range(const _K2& __k) const {return __tree_.__equal_range_unique(__k);}
#endif
};

#ifndef _LIBCPP_CXX03_LANG

template <class _Key, class _Compare, class _Allocator>
set<_Key, _Compare, _Allocator>::set(set&& __s, const allocator_type& __a)
    : __tree_(_VSTD::move(__s.__tree_), __a)
{
    if (__a != __s.get_allocator())
    {
        const_iterator __e = cend();
        while (!__s.empty())
            insert(__e, _VSTD::move(__s.__tree_.remove(__s.begin())->__value_));
    }
}

#endif  // _LIBCPP_CXX03_LANG

template <class _Key, class _Compare, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator==(const set<_Key, _Compare, _Allocator>& __x,
           const set<_Key, _Compare, _Allocator>& __y)
{
    return __x.size() == __y.size() && _VSTD::equal(__x.begin(), __x.end(), __y.begin());
}

template <class _Key, class _Compare, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator< (const set<_Key, _Compare, _Allocator>& __x,
           const set<_Key, _Compare, _Allocator>& __y)
{
    return _VSTD::lexicographical_compare(__x.begin(), __x.end(), __y.begin(), __y.end());
}

template <class _Key, class _Compare, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator!=(const set<_Key, _Compare, _Allocator>& __x,
           const set<_Key, _Compare, _Allocator>& __y)
{
    return !(__x == __y);
}

template <class _Key, class _Compare, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator> (const set<_Key, _Compare, _Allocator>& __x,
           const set<_Key, _Compare, _Allocator>& __y)
{
    return __y < __x;
}

template <class _Key, class _Compare, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>=(const set<_Key, _Compare, _Allocator>& __x,
           const set<_Key, _Compare, _Allocator>& __y)
{
    return !(__x < __y);
}

template <class _Key, class _Compare, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<=(const set<_Key, _Compare, _Allocator>& __x,
           const set<_Key, _Compare, _Allocator>& __y)
{
    return !(__y < __x);
}

// specialized algorithms:
template <class _Key, class _Compare, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
void
swap(set<_Key, _Compare, _Allocator>& __x,
     set<_Key, _Compare, _Allocator>& __y)
    _NOEXCEPT_(_NOEXCEPT_(__x.swap(__y)))
{
    __x.swap(__y);
}

template <class _Key, class _Compare = less<_Key>,
          class _Allocator = allocator<_Key> >
class _LIBCPP_TEMPLATE_VIS multiset
{
public:
    // types:
    typedef _Key                                      key_type;
    typedef key_type                                 value_type;
    typedef _Compare                                  key_compare;
    typedef key_compare                              value_compare;
    typedef _Allocator                                allocator_type;
    typedef value_type&                              reference;
    typedef const value_type&                        const_reference;

    static_assert((is_same<typename allocator_type::value_type, value_type>::value),
                  "Allocator::value_type must be same type as value_type");

private:
    typedef __tree<value_type, value_compare, allocator_type> __base;
    typedef allocator_traits<allocator_type>                  __alloc_traits;
    typedef typename __base::__node_holder                    __node_holder;

    __base __tree_;

public:
    typedef typename __base::pointer               pointer;
    typedef typename __base::const_pointer         const_pointer;
    typedef typename __base::size_type             size_type;
    typedef typename __base::difference_type       difference_type;
    typedef typename __base::const_iterator        iterator;
    typedef typename __base::const_iterator        const_iterator;
    typedef _VSTD::reverse_iterator<iterator>       reverse_iterator;
    typedef _VSTD::reverse_iterator<const_iterator> const_reverse_iterator;

    // construct/copy/destroy:
    _LIBCPP_INLINE_VISIBILITY
    multiset()
        _NOEXCEPT_(
            is_nothrow_default_constructible<allocator_type>::value &&
            is_nothrow_default_constructible<key_compare>::value &&
            is_nothrow_copy_constructible<key_compare>::value)
        : __tree_(value_compare()) {}

    _LIBCPP_INLINE_VISIBILITY
    explicit multiset(const value_compare& __comp)
        _NOEXCEPT_(
            is_nothrow_default_constructible<allocator_type>::value &&
            is_nothrow_copy_constructible<key_compare>::value)
        : __tree_(__comp) {}

    _LIBCPP_INLINE_VISIBILITY
    explicit multiset(const value_compare& __comp, const allocator_type& __a)
        : __tree_(__comp, __a) {}
    template <class _InputIterator>
        _LIBCPP_INLINE_VISIBILITY
        multiset(_InputIterator __f, _InputIterator __l,
                 const value_compare& __comp = value_compare())
        : __tree_(__comp)
        {
            insert(__f, __l);
        }

#if _LIBCPP_STD_VER > 11
        template <class _InputIterator>
        _LIBCPP_INLINE_VISIBILITY
        multiset(_InputIterator __f, _InputIterator __l, const allocator_type& __a)
            : multiset(__f, __l, key_compare(), __a) {}
#endif

    template <class _InputIterator>
        _LIBCPP_INLINE_VISIBILITY
        multiset(_InputIterator __f, _InputIterator __l,
                 const value_compare& __comp, const allocator_type& __a)
        : __tree_(__comp, __a)
        {
            insert(__f, __l);
        }

    _LIBCPP_INLINE_VISIBILITY
    multiset(const multiset& __s)
        : __tree_(__s.__tree_.value_comp(),
          __alloc_traits::select_on_container_copy_construction(__s.__tree_.__alloc()))
        {
            insert(__s.begin(), __s.end());
        }

    _LIBCPP_INLINE_VISIBILITY
    multiset& operator=(const multiset& __s)
        {
            __tree_ = __s.__tree_;
            return *this;
        }

#ifndef _LIBCPP_CXX03_LANG
    _LIBCPP_INLINE_VISIBILITY
    multiset(multiset&& __s)
        _NOEXCEPT_(is_nothrow_move_constructible<__base>::value)
        : __tree_(_VSTD::move(__s.__tree_)) {}

    multiset(multiset&& __s, const allocator_type& __a);
#endif  // _LIBCPP_CXX03_LANG
    _LIBCPP_INLINE_VISIBILITY
    explicit multiset(const allocator_type& __a)
        : __tree_(__a) {}
    _LIBCPP_INLINE_VISIBILITY
    multiset(const multiset& __s, const allocator_type& __a)
        : __tree_(__s.__tree_.value_comp(), __a)
        {
            insert(__s.begin(), __s.end());
        }

#ifndef _LIBCPP_CXX03_LANG
    _LIBCPP_INLINE_VISIBILITY
    multiset(initializer_list<value_type> __il, const value_compare& __comp = value_compare())
        : __tree_(__comp)
        {
            insert(__il.begin(), __il.end());
        }

    _LIBCPP_INLINE_VISIBILITY
    multiset(initializer_list<value_type> __il, const value_compare& __comp,
        const allocator_type& __a)
        : __tree_(__comp, __a)
        {
            insert(__il.begin(), __il.end());
        }

#if _LIBCPP_STD_VER > 11
    _LIBCPP_INLINE_VISIBILITY
    multiset(initializer_list<value_type> __il, const allocator_type& __a)
        : multiset(__il, key_compare(), __a) {}
#endif

    _LIBCPP_INLINE_VISIBILITY
    multiset& operator=(initializer_list<value_type> __il)
        {
            __tree_.__assign_multi(__il.begin(), __il.end());
            return *this;
        }

    _LIBCPP_INLINE_VISIBILITY
    multiset& operator=(multiset&& __s)
        _NOEXCEPT_(is_nothrow_move_assignable<__base>::value)
        {
            __tree_ = _VSTD::move(__s.__tree_);
            return *this;
        }
#endif  // _LIBCPP_CXX03_LANG

    _LIBCPP_INLINE_VISIBILITY
          iterator begin() _NOEXCEPT       {return __tree_.begin();}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator begin() const _NOEXCEPT {return __tree_.begin();}
    _LIBCPP_INLINE_VISIBILITY
          iterator end() _NOEXCEPT         {return __tree_.end();}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator end()   const _NOEXCEPT {return __tree_.end();}

    _LIBCPP_INLINE_VISIBILITY
          reverse_iterator rbegin() _NOEXCEPT
            {return reverse_iterator(end());}
    _LIBCPP_INLINE_VISIBILITY
    const_reverse_iterator rbegin() const _NOEXCEPT
        {return const_reverse_iterator(end());}
    _LIBCPP_INLINE_VISIBILITY
          reverse_iterator rend() _NOEXCEPT
            {return       reverse_iterator(begin());}
    _LIBCPP_INLINE_VISIBILITY
    const_reverse_iterator rend() const _NOEXCEPT
        {return const_reverse_iterator(begin());}

    _LIBCPP_INLINE_VISIBILITY
    const_iterator cbegin()  const _NOEXCEPT {return begin();}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator cend() const _NOEXCEPT {return end();}
    _LIBCPP_INLINE_VISIBILITY
    const_reverse_iterator crbegin() const _NOEXCEPT {return rbegin();}
    _LIBCPP_INLINE_VISIBILITY
    const_reverse_iterator crend() const _NOEXCEPT {return rend();}

    _LIBCPP_INLINE_VISIBILITY
    bool empty() const _NOEXCEPT {return __tree_.size() == 0;}
    _LIBCPP_INLINE_VISIBILITY
    size_type size() const _NOEXCEPT {return __tree_.size();}
    _LIBCPP_INLINE_VISIBILITY
    size_type max_size() const _NOEXCEPT {return __tree_.max_size();}

    // modifiers:
#ifndef _LIBCPP_CXX03_LANG
    template <class... _Args>
        _LIBCPP_INLINE_VISIBILITY
        iterator emplace(_Args&&... __args)
            {return __tree_.__emplace_multi(_VSTD::forward<_Args>(__args)...);}
    template <class... _Args>
        _LIBCPP_INLINE_VISIBILITY
        iterator emplace_hint(const_iterator __p, _Args&&... __args)
            {return __tree_.__emplace_hint_multi(__p, _VSTD::forward<_Args>(__args)...);}
#endif  // _LIBCPP_CXX03_LANG

    _LIBCPP_INLINE_VISIBILITY
    iterator insert(const value_type& __v)
        {return __tree_.__insert_multi(__v);}
    _LIBCPP_INLINE_VISIBILITY
    iterator insert(const_iterator __p, const value_type& __v)
        {return __tree_.__insert_multi(__p, __v);}

    template <class _InputIterator>
        _LIBCPP_INLINE_VISIBILITY
        void insert(_InputIterator __f, _InputIterator __l)
        {
            for (const_iterator __e = cend(); __f != __l; ++__f)
                __tree_.__insert_multi(__e, *__f);
        }

#ifndef _LIBCPP_CXX03_LANG
    _LIBCPP_INLINE_VISIBILITY
    iterator insert(value_type&& __v)
        {return __tree_.__insert_multi(_VSTD::move(__v));}

    _LIBCPP_INLINE_VISIBILITY
    iterator insert(const_iterator __p, value_type&& __v)
        {return __tree_.__insert_multi(__p, _VSTD::move(__v));}

    _LIBCPP_INLINE_VISIBILITY
    void insert(initializer_list<value_type> __il)
        {insert(__il.begin(), __il.end());}
#endif  // _LIBCPP_CXX03_LANG

    _LIBCPP_INLINE_VISIBILITY
    iterator  erase(const_iterator __p) {return __tree_.erase(__p);}
    _LIBCPP_INLINE_VISIBILITY
    size_type erase(const key_type& __k) {return __tree_.__erase_multi(__k);}
    _LIBCPP_INLINE_VISIBILITY
    iterator  erase(const_iterator __f, const_iterator __l)
        {return __tree_.erase(__f, __l);}
    _LIBCPP_INLINE_VISIBILITY
    void clear() _NOEXCEPT {__tree_.clear();}

    _LIBCPP_INLINE_VISIBILITY
    void swap(multiset& __s)
        _NOEXCEPT_(__is_nothrow_swappable<__base>::value)
        {__tree_.swap(__s.__tree_);}

    _LIBCPP_INLINE_VISIBILITY
    allocator_type get_allocator() const _NOEXCEPT {return __tree_.__alloc();}
    _LIBCPP_INLINE_VISIBILITY
    key_compare    key_comp()      const {return __tree_.value_comp();}
    _LIBCPP_INLINE_VISIBILITY
    value_compare  value_comp()    const {return __tree_.value_comp();}

    // set operations:
    _LIBCPP_INLINE_VISIBILITY
    iterator find(const key_type& __k)             {return __tree_.find(__k);}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator find(const key_type& __k) const {return __tree_.find(__k);}
#if _LIBCPP_STD_VER > 11
    template <typename _K2>
    _LIBCPP_INLINE_VISIBILITY
    typename _VSTD::enable_if<_VSTD::__is_transparent<_Compare, _K2>::value,iterator>::type
    find(const _K2& __k)                           {return __tree_.find(__k);}
    template <typename _K2>
    _LIBCPP_INLINE_VISIBILITY
    typename _VSTD::enable_if<_VSTD::__is_transparent<_Compare, _K2>::value,const_iterator>::type
    find(const _K2& __k) const                     {return __tree_.find(__k);}
#endif

    _LIBCPP_INLINE_VISIBILITY
    size_type      count(const key_type& __k) const
        {return __tree_.__count_multi(__k);}
#if _LIBCPP_STD_VER > 11
    template <typename _K2>
    _LIBCPP_INLINE_VISIBILITY
    typename enable_if<__is_transparent<_Compare, _K2>::value,size_type>::type
    count(const _K2& __k)                  {return __tree_.__count_multi(__k);}
#endif

    _LIBCPP_INLINE_VISIBILITY
    iterator lower_bound(const key_type& __k)
        {return __tree_.lower_bound(__k);}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator lower_bound(const key_type& __k) const
            {return __tree_.lower_bound(__k);}
#if _LIBCPP_STD_VER > 11
    template <typename _K2>
    _LIBCPP_INLINE_VISIBILITY
    typename _VSTD::enable_if<_VSTD::__is_transparent<_Compare, _K2>::value,iterator>::type
    lower_bound(const _K2& __k)       {return __tree_.lower_bound(__k);}

    template <typename _K2>
    _LIBCPP_INLINE_VISIBILITY
    typename _VSTD::enable_if<_VSTD::__is_transparent<_Compare, _K2>::value,const_iterator>::type
    lower_bound(const _K2& __k) const {return __tree_.lower_bound(__k);}
#endif

    _LIBCPP_INLINE_VISIBILITY
    iterator upper_bound(const key_type& __k)
            {return __tree_.upper_bound(__k);}
    _LIBCPP_INLINE_VISIBILITY
    const_iterator upper_bound(const key_type& __k) const
            {return __tree_.upper_bound(__k);}
#if _LIBCPP_STD_VER > 11
    template <typename _K2>
    _LIBCPP_INLINE_VISIBILITY
    typename _VSTD::enable_if<_VSTD::__is_transparent<_Compare, _K2>::value,iterator>::type
    upper_bound(const _K2& __k)       {return __tree_.upper_bound(__k);}
    template <typename _K2>
    _LIBCPP_INLINE_VISIBILITY
    typename _VSTD::enable_if<_VSTD::__is_transparent<_Compare, _K2>::value,const_iterator>::type
    upper_bound(const _K2& __k) const {return __tree_.upper_bound(__k);}
#endif

    _LIBCPP_INLINE_VISIBILITY
    pair<iterator,iterator>             equal_range(const key_type& __k)
            {return __tree_.__equal_range_multi(__k);}
    _LIBCPP_INLINE_VISIBILITY
    pair<const_iterator,const_iterator> equal_range(const key_type& __k) const
            {return __tree_.__equal_range_multi(__k);}
#if _LIBCPP_STD_VER > 11
    template <typename _K2>
    _LIBCPP_INLINE_VISIBILITY
    typename _VSTD::enable_if<_VSTD::__is_transparent<_Compare, _K2>::value,pair<iterator,iterator>>::type
    equal_range(const _K2& __k)       {return __tree_.__equal_range_multi(__k);}
    template <typename _K2>
    _LIBCPP_INLINE_VISIBILITY
    typename _VSTD::enable_if<_VSTD::__is_transparent<_Compare, _K2>::value,pair<const_iterator,const_iterator>>::type
    equal_range(const _K2& __k) const {return __tree_.__equal_range_multi(__k);}
#endif
};

#ifndef _LIBCPP_CXX03_LANG

template <class _Key, class _Compare, class _Allocator>
multiset<_Key, _Compare, _Allocator>::multiset(multiset&& __s, const allocator_type& __a)
    : __tree_(_VSTD::move(__s.__tree_), __a)
{
    if (__a != __s.get_allocator())
    {
        const_iterator __e = cend();
        while (!__s.empty())
            insert(__e, _VSTD::move(__s.__tree_.remove(__s.begin())->__value_));
    }
}

#endif  // _LIBCPP_CXX03_LANG

template <class _Key, class _Compare, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator==(const multiset<_Key, _Compare, _Allocator>& __x,
           const multiset<_Key, _Compare, _Allocator>& __y)
{
    return __x.size() == __y.size() && _VSTD::equal(__x.begin(), __x.end(), __y.begin());
}

template <class _Key, class _Compare, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator< (const multiset<_Key, _Compare, _Allocator>& __x,
           const multiset<_Key, _Compare, _Allocator>& __y)
{
    return _VSTD::lexicographical_compare(__x.begin(), __x.end(), __y.begin(), __y.end());
}

template <class _Key, class _Compare, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator!=(const multiset<_Key, _Compare, _Allocator>& __x,
           const multiset<_Key, _Compare, _Allocator>& __y)
{
    return !(__x == __y);
}

template <class _Key, class _Compare, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator> (const multiset<_Key, _Compare, _Allocator>& __x,
           const multiset<_Key, _Compare, _Allocator>& __y)
{
    return __y < __x;
}

template <class _Key, class _Compare, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator>=(const multiset<_Key, _Compare, _Allocator>& __x,
           const multiset<_Key, _Compare, _Allocator>& __y)
{
    return !(__x < __y);
}

template <class _Key, class _Compare, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
bool
operator<=(const multiset<_Key, _Compare, _Allocator>& __x,
           const multiset<_Key, _Compare, _Allocator>& __y)
{
    return !(__y < __x);
}

template <class _Key, class _Compare, class _Allocator>
inline _LIBCPP_INLINE_VISIBILITY
void
swap(multiset<_Key, _Compare, _Allocator>& __x,
     multiset<_Key, _Compare, _Allocator>& __y)
    _NOEXCEPT_(_NOEXCEPT_(__x.swap(__y)))
{
    __x.swap(__y);
}

_LIBCPP_END_NAMESPACE_STD

#endif  // _LIBCPP_SET
