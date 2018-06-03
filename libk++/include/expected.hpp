/*
expected.hpp

Copyright (c) 02 Yann BOUCHER (yann)

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
#ifndef EXPECTED_HPP
#define EXPECTED_HPP

#if 0

#include <utility.hpp>

#include <assert.h>

namespace kpp
{

template <class E>
class unexpected
{
public:
    unexpected() = delete;
    constexpr explicit unexpected(const E& e)
    { val = e; }
    constexpr explicit unexpected(E&& e)
    { val = std::move(e); }
    constexpr const E& value() const &
    { return val; }
    constexpr E& value() &
    { return val; }
    constexpr E&& value() &&
    { return std::move(val); }
    constexpr E const&& value() const&&
    { return std::move(val); }
private:
    E val; // exposition only
};

template <typename E>
unexpected<E> make_unexpected(E&& e)
{
    return unexpected<E>(std::forward<E>(e));
}

template <class E>
constexpr bool
operator==(const unexpected<E>&lhs , const unexpected<E>& rhs)
{
    return lhs.value() == rhs.value();
}

template <class E>
constexpr bool
operator!=(const unexpected<E>& lhs, const unexpected<E>& rhs)
{
    return !(lhs == rhs);
}

struct unexpect_t
{
    unexpect_t() = default;
};
inline constexpr unexpect_t unexpect{};

template <class T, class E>
class expected
{
    static_assert(!std::is_void_v<T>, "T cannot be void");

public:
    using value_type = T;
    using error_type = E;
    using unexpected_type = unexpected<E>;

    template <class U>
    struct rebind
    {
        using type = expected<U, error_type>;
    };

    // �.�.4.1, constructors
    constexpr expected()
        : m_val{}
    {}
    constexpr expected(const expected& rhs) noexcept
    {
        if (rhs)
        {
            set_value(*rhs);
        }
        else
        {
            set_unexpected(unexpected<E>(rhs.error()));
        }
        assert(bool(*this) == bool(rhs));
    }

    constexpr expected(expected&& rhs) noexcept
    {
        if (rhs)
        {
            set_value(std::move(*rhs));
        }
        else
        {
            set_unexpected(std::move(unexpected<E>(rhs.error())));
        }
        assert(bool(*this) == bool(rhs));
    }
    template <class U, class G>
    constexpr expected(const expected<U, G>& rhs)
    {
        if (rhs)
        {
            set_value(*rhs);
        }
        else
        {
            set_unexpected(unexpected<E>(rhs.error()));
        }
        assert(*this == rhs);
    }
    template <class U, class G>
    constexpr expected(expected<U, G>&& rhs)
    {
        if (rhs)
        {
            set_value(std::move(*rhs));
        }
        else
        {
            set_unexpected(std::move(unexpected<E>(rhs.error())));
        }
        assert(*this == rhs);
    }

    template <class U = T>
    constexpr expected(U&& v)
    {
        set_value(std::forward<U>(v));
        assert(*this);
    }

    template <class G = E>
    constexpr expected(unexpected<G> const& rgs)
    {
        set_unexpected(rgs);
        assert(!*this);
    }

    template <class G = E>
    constexpr expected(unexpected<G> && rgs)
    {
        set_unexpected(std::move(rgs));
        assert(!*this);
    }
    template <class... Args>
    constexpr explicit expected(unexpect_t, Args&&...args)
    {
        set_unexpected(unexpected<E>(std::forward<Args>(args)...));
        assert(!*this);
    }

    // �.�.4.2, destructor
    ~expected()
    {
        if (has_value())
        {
            m_val.~T();
        }
        else
        {
            m_unexpect.~unexpected_type();
        }
    }

    // �.�.4.3, assignment
    expected& operator=(const expected& rhs)
    {
        if (*this && rhs)
        {
            set_value(*rhs);
        }
        else if (!*this && !rhs)
        {
            set_unexpected(unexpected<E>(rhs.error()));
        }
        else if (*this && !rhs)
        {
            m_val.~T();
            set_unexpected(unexpected<E>(rhs.error()));
        }
        else // !*this && rhs
        {
            m_unexpect.~unexpected_type();
            set_value(*rhs);
        }
        assert(bool(*this) == bool(rhs));

        return *this;
    }

    expected& operator=(expected&& rhs)
    {
        if (*this && rhs)
        {
            set_value(std::move(*rhs));
        }
        else if (!*this && !rhs)
        {
            set_unexpected(std::move(unexpected<E>(rhs.error())));
        }
        else if (*this && !rhs)
        {
            m_val.~T();
            set_unexpected(std::move(unexpected<E>(rhs.error())));
        }
        else
        {
            m_unexpect.~unexpected_type();
            set_value(std::move(*rhs));
        }
        assert(bool(*this) == bool(rhs));

        return *this;
    }
    template <class U = T,
              typename = std::enable_if_t<!std::is_same_v<std::decay_t<U>, expected> &&
                                          !std::is_same_v<std::decay_t<U>, unexpected>>>
    expected& operator=(U&& rhs)
    {
        if (!*this)
        {
            m_val.~T();
        }
        set_value(std::forward<U>(rhs));
        assert(*this);

        return *this;
    }
    template <class G = E>
    expected& operator=(const unexpected<G>& rhs)
    {
        if (!*this)
        {
            m_unexpect.~unexpected_type();
        }
        set_unexpected(rhs);
        assert(!*this);

        return *this;
    }
    template <class G = E>
    expected& operator=(unexpected<G>&& rhs)
    {
        if (*this)
        {
            set_unexpected(std::move(rhs));
        }
        else
        {
            m_unexpect.~unexpected_type();
            set_unexpected(std::move(rhs));
        }
        assert(!*this);

        return *this;
    }

    template <class... Args>
    void emplace(Args&&... args)
    {
        if (!*this)
        {
            m_unexpect.~unexpected_type();
        }
        m_val = T{std::forward<Args>(args)...};
        m_has_val = true;
    }

    template <class U, class... Args>
    void emplace(std::initializer_list<U> il, Args&&... args)
    {
        if (!*this)
        {
            m_unexpect.~unexpected_type();
        }
        m_val = T{il, std::forward<Args>(args)...};
        m_has_val = true;
    }

    // �.�.4.5, observers
    constexpr const T* operator ->() const
    { assert(*this); return &m_val; }
    constexpr T* operator ->()
    { assert(*this); return &m_val; }
    constexpr const T& operator *() const&
    { assert(*this); return m_val; }
    constexpr T& operator *() &
    { assert(*this); return m_val; }
    constexpr const T&& operator *() const &&
    { assert(*this); return std::move(m_val); }
    constexpr T&& operator *() &&
    { assert(*this); return std::move(m_val); }
    constexpr explicit operator bool() const
    { return has_value(); }
    constexpr bool has_value() const
    { return m_has_val; }
    constexpr const T& value() const&
    { assert(has_value()); return m_val; }
    constexpr T& value() &
    { assert(has_value()); return m_val; }
    constexpr const T&& value() const &&
    { assert(has_value()); return std::move(m_val); }
    constexpr T&& value() &&
    { assert(has_value()); return std::move(m_val); }
    constexpr const E& error() const&
    { assert(!has_value()); return m_unexpect.value(); }
    constexpr E& error() &
    { assert(!has_value()); return m_unexpect.value(); }
    constexpr const E&& error() const &&
    { assert(!has_value()); return std::move(m_unexpect.value()); }
    constexpr E&& error() &&
    { assert(!has_value()); return std::move(m_unexpect.value()); }
    template <class U>
    constexpr T value_or(U&& v) const&
    { return *this ? value() : static_cast<T>(std::forward<U>(v)); }
    template <class U>
    T value_or(U&& v) &&
    { return *this ? std::move(value()) : static_cast<T>(std::forward<U>(v)); }

private:
    template <class U = T>
    void set_value(const U& val)
    {
        m_has_val = true;
        m_val = val;
    }

    template <class U = E>
    void set_unexpected(const unexpected<U>& val)
    {
        m_has_val = false;
        m_unexpect = val;
    }
private:
    bool m_has_val { true };
    union
    {
        value_type m_val;
        unexpected_type m_unexpect;
    };
};

template <class T, class E>
constexpr bool operator==(const expected<T, E>& x, const expected<T, E>& y)
{
    if (bool(x) != bool(y))
        return false;
    if (!bool(x))
        return unexpected<E>(x.error()) == unexpected<E>(y.error());

    if constexpr (std::is_void_v<T>)
    {
        return true;
    }
    else
    {
        return *x == *y;
    }
}

template <class T, class E> constexpr bool operator==(const expected<T, E>& x, const T& v)
{
    return *x == v;
}

template <class T, class E> constexpr bool operator==(const T& v, const expected<T, E>& x)
{
    return v == *x;
}

template <class T, class E> constexpr bool operator!=(const expected<T, E>& x, const T& v)
{
    return !(*x == v);
}

template <class T, class E> constexpr bool operator!=(const T& v, const expected<T, E>& x)
{
    return !(v == *x);
}

}

#else

#include "other_expected.hpp"

namespace kpp
{
using namespace tl;
}

#endif

#endif // EXPECTED_HPP
