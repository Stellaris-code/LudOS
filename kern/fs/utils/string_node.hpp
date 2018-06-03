/*
string_node.hpp

Copyright (c) 10 Yann BOUCHER (yann)

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
#ifndef STRING_NODE_HPP
#define STRING_NODE_HPP

#include "fs/vfs.hpp"

#include <functional.hpp>

struct string_node : public vfs::node
{
public:
    string_node(kpp::string name, std::function<kpp::string(size_t,size_t)> fun)
        : m_name(name)
    {
        m_callback = fun;
    }

    string_node(kpp::string name, std::function<kpp::string()> fun)
        : m_name(name)
    {
        m_callback = [fun](size_t off, size_t size)
        {
            auto str = fun();
            if (off >= str.length()) return kpp::string();
            return str.substr(off, size);
        };
    }

    string_node(kpp::string name, kpp::string str)
        : m_name(name)
    {
        m_callback = [str](size_t off, size_t size)
        {
            if (off >= str.length()) return kpp::string();
            return str.substr(off, size);
        };
    }


public:
    virtual Type type() const override { return File; }
    virtual kpp::string name() const override { return m_name; }
    virtual size_t size() const override { return 0; }

protected:
    [[nodiscard]] virtual kpp::expected<MemBuffer, vfs::FSError> read_impl(size_t offset, size_t size) const override
    {
        //auto substr = m_str.substr(offset, size);
        auto substr = m_callback(offset, size) + '\0';
        return MemBuffer{substr.begin(), substr.end()};
    }

private:
    kpp::string m_name;
    std::function<kpp::string(size_t, size_t)> m_callback;
};

#endif // STRING_NODE_HPP
