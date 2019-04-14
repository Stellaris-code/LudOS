/*
atomic.hpp

Copyright (c) 14 Yann BOUCHER (yann)

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
#ifndef ATOMIC_HPP
#define ATOMIC_HPP

#define atomic_fetch_add(x, v) __atomic_fetch_add(x, v, __ATOMIC_SEQ_CST)
#define atomic_fetch_sub(x, v) __atomic_fetch_sub(x, v, __ATOMIC_SEQ_CST)
#define atomic_add_fetch(x, v) __atomic_add_fetch(x, v, __ATOMIC_SEQ_CST)
#define atomic_sub_fetch(x,v ) __atomic_sub_fetch(x, v, __ATOMIC_SEQ_CST)

#define atomic_inc(x) __atomic_fetch_add(x, 1, __ATOMIC_SEQ_CST)
#define atomic_dec(x) __atomic_fetch_sub(x, 1, __ATOMIC_SEQ_CST)

#define atomic_load(x) __atomic_load_n(x, __ATOMIC_SEQ_CST)
#define atomic_store(x, val) __atomic_store_n(x, val, __ATOMIC_SEQ_CST)

#endif // ATOMIC_HPP
