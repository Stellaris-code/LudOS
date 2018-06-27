/*
align.hpp

Copyright (c) 24 Yann BOUCHER (yann)

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
#ifndef ALIGN_HPP
#define ALIGN_HPP

#include <stdint.h>

template <typename Ptr = void*>
inline Ptr aligned(Ptr ptr, size_t alignement)
{
    uintptr_t result = reinterpret_cast<uintptr_t>(ptr);
    if (result % alignement != 0)
        result += alignement - result % alignement;

    return reinterpret_cast<Ptr>(result);
}

#define ALIGN_STACK(align) \
    volatile void* __esp__; \
    volatile uint8_t __align_buf__ [(align) - ((uintptr_t)&__esp__ & ((align)-1))]; \
    (void)__align_buf__;

#endif // ALIGN_HPP
