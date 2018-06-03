/*
dissasembly.hpp

Copyright (c) 12 Yann BOUCHER (yann)

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
#ifndef DISSASEMBLY_HPP
#define DISSASEMBLY_HPP

#include <stdint.h>

#include <kstring/kstring.hpp>

#include "utils/membuffer.hpp"

struct DisasmInfo
{
    kpp::string str;
    MemBuffer bytes;
    size_t len;
};

DisasmInfo get_disasm(const uint8_t *ptr);

inline const uint8_t *next_ins(const uint8_t* ptr, size_t instructions = 1)
{
    for (size_t i { 0 }; i < instructions; ++i)
    {
        auto info = get_disasm(ptr);
        ptr += info.len;
    }

    return ptr;
}

#endif // DISSASEMBLY_HPP
