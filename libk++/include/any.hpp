/*
any.hpp

Copyright (c) 30 Yann BOUCHER (yann)

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
#ifndef KPP_ANY_HPP
#define KPP_ANY_HPP

// Credit to https://gist.github.com/Jefffrey/9202235

#include <memory.hpp>
#include <type_traits.hpp>

#include <types/typeid.hpp>

#include "panic.hpp"

namespace kpp
{

class any
{
    using id = size_t;

    template<typename T>
    struct type { static void id() { } };

    template<typename T>
    static id type_id() { return reinterpret_cast<id>(&type<T>::id); }

    template<typename T>
    using decay = typename std::decay<T>::type;

    template<typename T>
    using none = typename std::enable_if<!std::is_same<any, T>::value>::type;

    struct base
    {
        virtual ~base() { }
        virtual bool is(id) const = 0;
        virtual base *copy() const = 0;
    } *p = nullptr;

    template<typename T>
    struct data : base, std::tuple<T>
    {
        using std::tuple<T>::tuple;

        T       &get()      & { return std::get<0>(*this); }
        T const &get() const& { return std::get<0>(*this); }

        bool is(id i) const override { return i == type_id<T>(); }
        base *copy()  const override { return new data{get()}; }
    };

    template<typename T>
    T &stat() { return static_cast<data<T>&>(*p).get(); }

    template<typename T>
    T const &stat() const { return static_cast<data<T> const&>(*p).get(); }

    template<typename T>
    T &dyn() { return dynamic_cast<data<T>&>(*p).get(); }

    template<typename T>
    T const &dyn() const { return dynamic_cast<data<T> const&>(*p).get(); }

public:
     any() { }
    ~any() { delete p; }

    any(any &&s)      : p{s.p} { s.p = nullptr; }
    any(any const &s) : p{s.p->copy()} { }

    template<typename T, typename U = decay<T>, typename = none<U>>
    any(T &&x) : p{new data<U>{std::forward<T>(x)}} { }

    any &operator=(any s) { swap(*this, s); return *this; }

    friend void swap(any &s, any &r) { std::swap(s.p, r.p); }

    void clear() { delete p; p = nullptr; }

    bool empty() const { return p; }

    template<typename T>
    bool is() const { return p ? p->is(type_id<T>()) : false; }

    template<typename T> T      &&_()     && { return std::move(stat<T>()); }
    template<typename T> T       &_()      & { return stat<T>(); }
    template<typename T> T const &_() const& { return stat<T>(); }

    template<typename T> T      &&cast()     && { return std::move(dyn<T>()); }
    template<typename T> T       &cast()      & { return dyn<T>(); }
    template<typename T> T const &cast() const& { return dyn<T>(); }

    template<typename T> operator T     &&()     && { return std::move(_<T>()); }
    template<typename T> operator T      &()      & { return _<T>(); }
    template<typename T> operator T const&() const& { return _<T>(); }
};
}

#endif // ANY_HPP
