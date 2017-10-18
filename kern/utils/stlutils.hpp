/*
stlutils.hpp

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
#ifndef STLUTILS_HPP
#define STLUTILS_HPP

#include <string.hpp>
#include <vector.hpp>
#include <functional.hpp>

template <class ContainerT = std::vector<std::string>>
inline ContainerT tokenize(const std::string& str, const std::string& delimiters = " ", bool trimEmpty = false)
{
    ContainerT tokens;

    std::string::size_type pos, lastPos = 0, length = str.length();

    using value_type = typename ContainerT::value_type;
    using size_type  = typename ContainerT::size_type;

    while(lastPos < length + 1)
    {
        pos = str.find_first_of(delimiters, lastPos);
        if(pos == std::string::npos)
        {
            pos = length;
        }

        if(pos != lastPos || !trimEmpty)
            tokens.push_back(value_type(str.data()+lastPos,
                                        (size_type)pos-lastPos ));

        lastPos = pos + 1;
    }

    return tokens;
}

template <typename Container = std::vector<std::string>>
inline std::string join(const Container& cont, const std::string& join_str)
{
    std::string result;
    for (int i { 0 }; i < int(cont.size())-1; ++i)
    {
        result += cont[i];
        result += join_str;
    }
    result += cont.back();

    return result;
}

template <typename Cont>
Cont inline merge(const Cont& lhs, const Cont& rhs)
{
    Cont cont = lhs;
    cont.insert(cont.end(),
        rhs.begin(),
        rhs.end());

    return cont;
}

std::string inline trim(const std::string& str)
{
    return std::string(str.c_str(), strlen(str.c_str()));
}

template <typename Cont>
std::vector<Cont> split(const Cont& cont, size_t chunk_size, bool fill = false)
{
    std::vector<Cont> chunks;

    size_t base = 0;

    while (base + chunk_size <= cont.size())
    {
        chunks.emplace_back(cont.begin() + base, cont.begin() + base + chunk_size);

        base += chunk_size;
    }

    if (base < cont.size())
    {
        chunks.emplace_back(cont.begin() + base, cont.end());
        if (fill)
        {
            chunks.back().resize(chunk_size);
        }
    }

    return chunks;
}

template <typename T, typename U>
std::vector<U> map(const std::vector<T>& cont, std::function<U(const T&)> fun)
{
    std::vector<U> result(cont.size());
    std::transform(cont.begin(), cont.end(), result.begin(), fun);

    return result;
}

#endif // STLUTILS_HPP
