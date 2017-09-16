/*
unique_ptr.hpp

Copyright (c) 16 Yann BOUCHER (yann)

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
#ifndef UNIQUE_PTR_HPP
#define UNIQUE_PTR_HPP


// can be replaced by other error mechanism
#include <assert.h>
#define SHARED_ASSERT(x)    assert(x)


template <typename T>
inline T& move(T& v)
{
    return v;
}

template<class T>
class unique_ptr
{
public:
    /// The type of the managed object, aliased as member type
    typedef T element_type;

    /// @brief Default constructor
    unique_ptr() nothrow : // never throws
        px(nullptr)
    {
    }
    /// @brief Constructor with the provided pointer to manage
    explicit unique_ptr(T* p) nothrow : // never throws
        px(p)
    {
    }
    /// @brief Copy constructor to convert from another pointer type
    /* TODO MSVC error C2248: 'unique_ptr<B>::px' : unique_ptr<A> cannot access private member declared in class 'unique_ptr<B>'
    template <class U>
    unique_ptr(const unique_ptr<U>& ptr) nothrow : // never throws
        px(static_cast<typename unique_ptr<T>::element_type*>(ptr.px))
    {
        const_cast<unique_ptr<U>&>(ptr).px = nullptr; // const-cast to force ownership transfer!
    }
    */
    /// @brief Copy constructor (used by the copy-and-swap idiom)
    unique_ptr(const unique_ptr& ptr) nothrow : // never throws
        px(ptr.px)
    {
        const_cast<unique_ptr&>(ptr).px = nullptr; // const-cast to force ownership transfer!
    }
    /// @brief Assignment operator using the copy-and-swap idiom (copy constructor and swap method)
    unique_ptr& operator=(unique_ptr ptr) nothrow // never throws
    {
        swap(ptr);
        return *this;
    }
    /// @brief the destructor releases its ownership and destroy the object
    inline ~unique_ptr() nothrow // never throws
    {
        destroy();
    }
    /// @brief this reset releases its ownership and destroy the object
    inline void reset() nothrow // never throws
    {
        destroy();
    }
    /// @brief this reset release its ownership and re-acquire another one
    void reset(T* p) nothrow // never throws
    {
        SHARED_ASSERT((nullptr == p) || (px != p)); // auto-reset not allowed
        destroy();
        px = p;
    }

    /// @brief Swap method for the copy-and-swap idiom (copy constructor and swap method)
    void swap(unique_ptr& lhs) nothrow // never throws
    {
        std::swap(px, lhs.px);
    }

    /// @brief release the ownership of the px pointer without destroying the object!
    inline void release() nothrow // never throws
    {
        px = nullptr;
    }

    // reference counter operations :
    inline operator bool() const nothrow // never throws
    {
        return (nullptr != px); // TODO nullptrptr
    }

    // underlying pointer operations :
    inline T& operator*()  const nothrow // never throws
    {
        SHARED_ASSERT(nullptr != px);
        return *px;
    }
    inline T* operator->() const nothrow // never throws
    {
        SHARED_ASSERT(nullptr != px);
        return px;
    }
    inline T* get()  const nothrow // never throws
    {
        // no assert, can return nullptr
        return px;
    }

private:
    /// @brief release the ownership of the px pointer and destroy the object
    inline void destroy() nothrow // never throws
    {
        delete px;
        px = nullptr;
    }

    /// @brief hack: const-cast release the ownership of the px pointer without destroying the object!
    inline void release() const nothrow // never throws
    {
        px = nullptr;
    }

private:
    T* px; //!< Native pointer
};


// comparaison operators
template<class T, class U> inline bool operator==(const unique_ptr<T>& l, const unique_ptr<U>& r) nothrow // never throws
{
    return (l.get() == r.get());
}
template<class T, class U> inline bool operator!=(const unique_ptr<T>& l, const unique_ptr<U>& r) nothrow // never throws
{
    return (l.get() != r.get());
}
template<class T, class U> inline bool operator<=(const unique_ptr<T>& l, const unique_ptr<U>& r) nothrow // never throws
{
    return (l.get() <= r.get());
}
template<class T, class U> inline bool operator<(const unique_ptr<T>& l, const unique_ptr<U>& r) nothrow // never throws
{
    return (l.get() < r.get());
}
template<class T, class U> inline bool operator>=(const unique_ptr<T>& l, const unique_ptr<U>& r) nothrow // never throws
{
    return (l.get() >= r.get());
}
template<class T, class U> inline bool operator>(const unique_ptr<T>& l, const unique_ptr<U>& r) nothrow // never throws
{
return (l.get() > r.get());

#endif // UNIQUE_PTR_HPP
