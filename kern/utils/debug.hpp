/*
debug.hpp

Copyright (c) 13 Yann BOUCHER (yann)

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
#ifndef DEBUG_HPP
#define DEBUG_HPP

#define BREAKPOINT() do { __asm__ __volatile__ ("int3") } while(0)

#include <functional.hpp>

#include <stdio.h>

#include <kstring/kstring.hpp>

#include "time/time.hpp"

void benchmark(const std::function<void()>& fun, const kpp::string& description, size_t repetitions = 100)
{
    kprintf("Benchmarking function %s\n", description.c_str());

    double beg = Time::uptime();
    while (repetitions--)
    {
        fun();
    }
    double end = Time::uptime();

    kprintf("Elapsed secs : %f\n", end - beg);
}

#endif // DEBUG_HPP
