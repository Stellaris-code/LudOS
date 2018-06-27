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

#include <vector.hpp>
#include <functional.hpp>

#include <utils/gsl/gsl_span.hpp>

#include <kstring/kstring.hpp>

#include <ctype.h>

template <class ContainerT = std::vector<kpp::string>>
inline ContainerT tokenize(const kpp::string& str, const kpp::string& delimiters = " ", bool trimEmpty = false)
{
    ContainerT tokens;

    kpp::string::size_type pos, lastPos = 0, length = str.length();

    using value_type = typename ContainerT::value_type;

    while(lastPos < length + 1)
    {
        pos = str.find_first_of(delimiters, lastPos);
        if(pos == kpp::string::npos)
        {
            pos = length;
        }

        if(pos != lastPos || !trimEmpty)
            tokens.push_back(value_type(str.data()+lastPos,
                                        pos-lastPos ));

        lastPos = pos + 1;
    }

    return tokens;
}

template <typename T, typename U, typename ContainerT = std::vector<T>, typename ContainerU = std::vector<U>>
inline ContainerU map(const ContainerT& in, std::function<U(const T&)> fun)
{
    ContainerU cont(in.size());

    for (size_t i { 0 }; i < in.size(); ++i)
    {
        cont[i] = fun(in[i]);
    }

    return cont;
}

template <typename Container = std::vector<kpp::string>>
inline kpp::string join(const Container& cont, const kpp::string& join_str)
{
    if (cont.empty())
    {
        return "";
    }

    kpp::string result;
    for (int i { 0 }; i < int(cont.size())-1; ++i)
    {
        result += cont[i];
        result += join_str;
    }
    result += cont.back();

    return result;
}

template <typename Cont>
inline void merge(Cont& lhs, const Cont& rhs)
{
    size_t old_size = lhs.size();
    lhs.resize(old_size + rhs.size());

    std::copy(rhs.begin(), rhs.end(), lhs.begin() + old_size);
}

kpp::string inline trim_zstr(const kpp::string& str)
{
    return kpp::string(str.c_str(), strlen(str.c_str()));
}

kpp::string inline trim_right(kpp::string str)
{
    while (!str.empty() && isspace(str.back()))
    {
        str.pop_back();
    }
    return str;
}

kpp::string inline trim_left(kpp::string str)
{
    while (!str.empty() && isspace(str.front()))
    {
        str.erase(0, 1);
    }
    return str;
}

kpp::string inline trim(kpp::string str)
{
    return trim_right(trim_left(std::move(str)));
}

template <class ContainerT = std::vector<kpp::string>>
inline ContainerT quote_tokenize(const kpp::string& str)
{
    auto tokens = tokenize(str, "\"");

    ContainerT result;

    for (size_t i { 0 }; i < tokens.size(); ++i)
    {
        bool is_in_quotes = i % 2;

        kpp::string el = tokens[i];

        if (!is_in_quotes)
        {
            el = trim(el);
            for (auto tok : tokenize(el, " ", true))
            {
                result.emplace_back(tok);
            }
        }
        else
        {
            result.emplace_back(el);
        }
    }

    return result;
}

template <typename Cont, bool fill = false>
inline std::vector<Cont> split(const Cont& cont, size_t chunk_size)
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
        if constexpr (fill)
        {
            chunks.back().resize(chunk_size);
        }
    }

    return chunks;
}

template <typename T>
inline std::vector<gsl::span<T>> split(gsl::span<T> cont, size_t chunk_size)
{
    std::vector<gsl::span<T>> chunks;

    typename gsl::span<T>::index_type base = 0;

    while (base + (int)chunk_size <= cont.size())
    {
        chunks.emplace_back(cont.subspan(base, (int)chunk_size));

        base += (int)chunk_size;
    }

    if (base < cont.size())
    {
        chunks.emplace_back(cont.subspan(base));
    }

    return chunks;
}

template <typename T, typename U>
inline std::vector<U> map(const std::vector<T>& cont, std::function<U(const T&)> fun)
{
    std::vector<U> result(cont.size());
    std::transform(cont.begin(), cont.end(), result.begin(), fun);

    return result;
}

inline kpp::string strtolower(kpp::string str)
{
    std::transform(str.begin(), str.end(),str.begin(), ::tolower);
    return str;
}

inline kpp::string strtoupper(kpp::string str)
{
    std::transform(str.begin(), str.end(),str.begin(), ::toupper);
    return str;
}

template <typename T, typename U>
inline T& closest(const T& base, std::vector<T>& values, std::function<U(const T& lhs, const T& rhs)> comp)
{
    assert(!values.empty());

    size_t closest_idx = 0;
    U smallest_value = comp(base, values[0]);

    for (size_t i { 1 }; i < values.size(); ++i)
    {
        U value = comp(base, values[i]);
        if (value == 0)
        {
            return values[i];
        }
        if (value < smallest_value)
        {
            smallest_value = value;
            closest_idx = i;
        }
    }

    return values[closest_idx];
}

inline kpp::string format(const kpp::string& format, const std::vector<std::pair<kpp::string, kpp::string>>& values)
{
    auto tokens = tokenize(format, "{}");
    bool in_param = false;

    kpp::string result;

    for (auto tok : tokens)
    {
        if (in_param)
        {
            in_param = false;

            auto it = std::find_if(values.begin(), values.end(),
                                   [tok](const std::pair<kpp::string, kpp::string>& p)
                    {return tok == p.first;});
            if (it != values.end())
            {
                result += it->second;

                continue;
            }
        }
        else
        {
            in_param = true;
        }

        result += tok;
    }

    return result;
}

template <typename T, typename Cont>
typename Cont::value_type* find(Cont& cont, const T& val)
{
    auto it = std::find(std::begin(cont), std::end(cont), val);
    if (it == std::end(cont)) return nullptr;
    else return &(*it);
}

template <typename T, typename Cont>
const typename Cont::value_type* find(const Cont& cont, const T& val)
{
    auto it = std::find(std::begin(cont), std::end(cont), val);
    if (it == std::end(cont)) return nullptr;
    else return &(*it);
}

template <typename T, typename Cont>
void erase(Cont& cont, const T& val)
{
    cont.erase(std::remove(std::begin(cont), std::end(cont), val), std::end(cont));
}

template< typename ContainerT, typename PredicateT >
void erase_if( ContainerT& items, const PredicateT& predicate ) {
  for( auto it = items.begin(); it != items.end(); ) {
    if( predicate(*it) ) it = items.erase(it);
    else ++it;
  }
};

#endif // STLUTILS_HPP
