/*
demangle.hpp

Copyright (c) 31 Yann BOUCHER (yann)

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
#ifndef DEMANGLE_HPP
#define DEMANGLE_HPP

#include <kstring/kstring.hpp>

#include <cxxabi.h>

#include "utils/logging.hpp"

inline const char* demangle(const kpp::string& symbol)
{
    static char buffer[2048];
    size_t len = sizeof(buffer);

    int status = -1;
    char* demangled_cstr = abi::__cxa_demangle(symbol.c_str(), buffer, &len, &status);

    if (status == 0)
    {
        return demangled_cstr;
    }

    return symbol.c_str();
}

#endif // DEMANGLE_HPP
