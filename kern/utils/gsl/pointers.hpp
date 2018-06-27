///////////////////////////////////////////////////////////////////////////////
//
// Copyright (c) 2015 Microsoft Corporation. All rights reserved.
//
// This code is licensed under the MIT License (MIT).
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef GSL_POINTERS_H
#define GSL_POINTERS_H

#include <utils/gsl/gsl_assert.hpp>  // for Ensures, Expects

#include <algorithm.hpp>    // for forward
#include <memory.hpp>       // for shared_ptr, unique_ptr
#include <type_traits.hpp>  // for enable_if_t, is_convertible, is_assignable

#if defined(_MSC_VER) && _MSC_VER < 1910
#pragma push_macro("constexpr")
#define constexpr /*constexpr*/

#endif                          // defined(_MSC_VER) && _MSC_VER < 1910

namespace gsl
{

typedef decltype(nullptr) nullptr_t;

//
// GSL.owner: ownership pointers
//
using std::unique_ptr;
using std::shared_ptr;

//
// owner
//
// owner<T> is designed as a bridge for code that must deal directly with owning pointers for some reason
//
// T must be a pointer type
// - disallow construction from any type other than pointer type
//
template <class T, class = std::enable_if_t<std::is_pointer<T>::value>>
using owner = T;

//
// not_null
//
// Restricts a pointer or smart pointer to only hold non-null values.
//
// Has zero size overhead over T.
//
// If T is a pointer (i.e. T == U*) then
// - allow construction from U*
// - disallow construction from nullptr_t
// - disallow default construction
// - ensure construction from null U* fails
// - allow implicit conversion to U*
//
template <class T>
class not_null
{
public:
    static_assert(std::is_assignable<T&, nullptr_t>::value, "T cannot be assigned nullptr.");

    template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    constexpr not_null(U&& u) : ptr_(std::forward<U>(u))
    {
        assert(ptr_ != nullptr);
    }

    template <typename U, typename = std::enable_if_t<std::is_convertible<U, T>::value>>
    constexpr not_null(const not_null<U>& other) : not_null(other.get())
    {
    }

    not_null(const not_null& other) = default;
    not_null& operator=(const not_null& other) = default;

    constexpr T get() const
    {
        assert(ptr_ != nullptr);
        return ptr_;
    }

    constexpr operator T() const { return get(); }
    constexpr T operator->() const { return get(); }
    constexpr decltype(auto) operator*() const { return *get(); }

    // prevents compilation when someone attempts to assign a null pointer constant
    not_null(nullptr_t) = delete;
    not_null& operator=(nullptr_t) = delete;

    // unwanted operators...pointers only point to single objects!
    not_null& operator++() = delete;
    not_null& operator--() = delete;
    not_null operator++(int) = delete;
    not_null operator--(int) = delete;
    not_null& operator+=(ptrdiff_t) = delete;
    not_null& operator-=(ptrdiff_t) = delete;
    void operator[](ptrdiff_t) const = delete;

private:
    T ptr_;
};

template <class T, class U>
auto operator==(const not_null<T>& lhs, const not_null<U>& rhs) -> decltype(lhs.get() == rhs.get())
{
    return lhs.get() == rhs.get();
}

template <class T, class U>
auto operator!=(const not_null<T>& lhs, const not_null<U>& rhs) -> decltype(lhs.get() != rhs.get())
{
    return lhs.get() != rhs.get();
}

template <class T, class U>
auto operator<(const not_null<T>& lhs, const not_null<U>& rhs) -> decltype(lhs.get() < rhs.get())
{
    return lhs.get() < rhs.get();
}

template <class T, class U>
auto operator<=(const not_null<T>& lhs, const not_null<U>& rhs) -> decltype(lhs.get() <= rhs.get())
{
    return lhs.get() <= rhs.get();
}

template <class T, class U>
auto operator>(const not_null<T>& lhs, const not_null<U>& rhs) -> decltype(lhs.get() > rhs.get())
{
    return lhs.get() > rhs.get();
}

template <class T, class U>
auto operator>=(const not_null<T>& lhs, const not_null<U>& rhs) -> decltype(lhs.get() >= rhs.get())
{
    return lhs.get() >= rhs.get();
}

// more unwanted operators
template <class T, class U>
ptrdiff_t operator-(const not_null<T>&, const not_null<U>&) = delete;
template <class T>
not_null<T> operator-(const not_null<T>&, ptrdiff_t) = delete;
template <class T>
not_null<T> operator+(const not_null<T>&, ptrdiff_t) = delete;
template <class T>
not_null<T> operator+(ptrdiff_t, const not_null<T>&) = delete;

} // namespace gsl

namespace std
{
template <class T>
struct hash<gsl::not_null<T>>
{
    size_t operator()(const gsl::not_null<T>& value) const { return hash<T>{}(value); }
};

} // namespace std

#if defined(_MSC_VER) && _MSC_VER < 1910
#undef constexpr
#pragma pop_macro("constexpr")

#endif // defined(_MSC_VER) && _MSC_VER < 1910

#endif // GSL_POINTERS_H

