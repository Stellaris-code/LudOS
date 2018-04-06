/*
user_ptr.hpp

Copyright (c) 05 Yann BOUCHER (yann)

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
#ifndef USER_PTR_HPP
#define USER_PTR_HPP

#include <stdint.h>
#include <assert.h>

#include "mem/memmap.hpp"

template <typename T>
struct user_ptr
{
    bool check(size_t size = sizeof(T))
    {
        return VM::check_user_ptr(ptr, size);
    }

    T* get()
    {
        assert(check());
        return ptr;
    }

    T* bypass()
    {
        return ptr;
    }

private:
    T* ptr;
};
static_assert(sizeof(user_ptr<void>) == sizeof(uintptr_t));

#endif // USER_PTR_HPP
