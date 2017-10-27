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

#include <string.hpp>
#include <vector.hpp>

#include "utils/stlutils.hpp"

inline std::string filename(const std::string& path)
{
    return tokenize(path, "/").back();
}

inline std::string stem(const std::string& path)
{
    auto file = filename(path);
    auto toks = tokenize(file, ".");
    toks.pop_back(); // remove extension
    return join(toks, ".");
}

inline std::string extension(const std::string& path)
{
    auto file = filename(path);
    return tokenize(file, ".").back();
}

inline std::string parent_path(const std::string& path)
{
    auto toks = tokenize(path, "/");
    toks.pop_back(); // remove file
    return join(toks, "/") + "/";
}

inline std::vector<std::string> dir_list(const std::string& path)
{
    return tokenize(parent_path(path), "/", true);
}

inline std::vector<std::string> path_list(const std::string& path)
{
    return tokenize(path, "/", true);
}

#endif // PATHUTILS_HPP
