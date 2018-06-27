/*
pathutils.hpp

Copyright (c) 01 Yann BOUCHER (yann)

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
#ifndef PATHUTILS_HPP
#define PATHUTILS_HPP

#include <vector.hpp>

#include "utils/stlutils.hpp"

// TODO : use string_view

inline kpp::string filename(const kpp::string& path)
{
    return tokenize(path, "/").back();
}

inline kpp::string stem(const kpp::string& path)
{
    auto file = filename(path);
    auto toks = tokenize(file, ".");
    toks.pop_back(); // remove extension
    return join(toks, ".");
}

inline kpp::string extension(const kpp::string& path)
{
    auto file = filename(path);
    return tokenize(file, ".").back();
}

inline kpp::string parent_path(const kpp::string& path)
{
    auto toks = tokenize(path, "/");
    toks.pop_back(); // remove file
    return join(toks, "/") + "/";
}

inline std::vector<kpp::string> dir_list(const kpp::string& path)
{
    return tokenize(parent_path(path), "/", true);
}

inline std::vector<kpp::string> path_list(const kpp::string& path)
{
    return tokenize(path, "/", true);
}

#endif // PATHUTILS_HPP
