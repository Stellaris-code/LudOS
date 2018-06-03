/*
optional.hpp

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
#ifndef OPTIONAL_HPP
#define OPTIONAL_HPP

#include <stdint.h>
#include <assert.h>

#include <type_traits.hpp>

#if 0

#include_next <optional.hpp>

namespace kpp
{
template <typename T>
using optional = std::optional<T>;
}

#else

namespace kpp
{

struct nullopt_t {
    explicit constexpr nullopt_t(int) {}
};

inline constexpr nullopt_t nullopt { 0 };

template <typename T>
class optional
{
public:
    constexpr optional() {}
    constexpr optional(nullopt_t) {}
    constexpr optional(const optional& other) { if (other) allocate(other.value()); }
    constexpr optional(optional&& other) { if (other) allocate(std::move(other.value())); }
    template <typename U = T>
    constexpr optional(U&& val) { allocate(std::forward<U>(val)); }

    ~optional() { reset(); }

    optional& operator=(nullopt_t) noexcept
    {
        reset();

        return *this;
    }
    optional& operator=( const optional& other )
    {
        if (other && !has_value())
        {
            allocate(other.value());
        }
        else if (other && has_value())
        {
            value() = other.value();
        }
        else if (!other && has_value())
        {
            reset();
        }

        return *this;
    }
    optional& operator=(optional&& other )
    {
        if (other && !has_value())
        {
            allocate(std::move(other.value()));
        }
        else if (other && has_value())
        {
            value() = std::move(other.value());
        }
        else if (!other && has_value())
        {
            reset();
        }

        return *this;
    }
    template<typename U = T>
    optional& operator=(U&& value)
    {
        if (!has_value())
        {
            allocate(std::forward<U>(value));
        }
        else
        {
            this->value() = std::forward<U>(value);
        }

        return *this;
    }

    constexpr bool has_value() const { return m_has_value; }
    constexpr explicit operator bool() const { return has_value(); }

    void reset() { if (has_value()) deallocate(); }

    constexpr T& value() &
    { assert(has_value()); return (T&)m_value_data; }
    constexpr const T& value() const &
    { assert(has_value()); return (T&)m_value_data; }

    constexpr T&& value() &&
    { assert(has_value()); return std::move(m_value_data); }
    constexpr const T&& value() const &&
    { assert(has_value()); return std::move(m_value_data); }

    template< class U >
    constexpr T value_or( U&& default_value ) const&
    { bool(*this) ? **this : static_cast<T>(std::forward<U>(default_value)); }
    template< class U >
    constexpr T value_or( U&& default_value ) &&
    { bool(*this) ? std::move(**this) : static_cast<T>(std::forward<U>(default_value)); }

    constexpr T& operator*() & { return value(); }
    constexpr const T&& operator*() const&& { return std::move(value()); }
    constexpr T&& operator*() && { return std::move(value()); }

    constexpr const T* operator->() const { return &value(); }
    constexpr T* operator->() { return &value(); }
    constexpr const T& operator*() const& { return &value(); }

private:
    template <typename... Arg>
    constexpr void allocate(Arg&&... args)
    {
        new (m_value_data) T(std::forward<Arg>(args)...);
        m_has_value = true;
    }

    void deallocate()
    {
        if constexpr (!std::is_trivially_destructible_v<T>)
        {
            if (has_value())
            {
                value().~T();
            }
        }
        m_has_value = false;
    }

private:
    bool m_has_value { false };
    alignas(alignof(T))
    uint8_t m_value_data[sizeof(T)];
};

}

#endif

#endif // OPTIONAL_HPP
