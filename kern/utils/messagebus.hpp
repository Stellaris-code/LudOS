/*
messagebus.hpp

Copyright (c) 23 Yann BOUCHER (yann)

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
#ifndef MESSAGEBUS_HPP
#define MESSAGEBUS_HPP

#include <functional.hpp>
#include <unordered_map.hpp>
#include <list.hpp>
#include <typeindex.hpp>
#include <any.hpp>
#include <optional.hpp>

class MessageBus
{
public:
    using Handle = std::pair<std::type_index, std::list<std::any>::const_iterator>;

public:
    template <typename T>
    static Handle register_handler(std::function<void(const T&)> handler);

    static void remove_handler(const Handle& handle);

    template <typename T>
    static void send(const T& event);

public:
    struct RAIIHandle
    {
        RAIIHandle() = default;
        ~RAIIHandle()
        {
            if (handle) remove_handler(*handle);
        }
        RAIIHandle(const Handle& ihandle)
        {
            handle = ihandle;
        }
        RAIIHandle& operator=(const Handle& other)
        {
            handle = other;
            return *this;
        }

        std::optional<Handle> handle {};
    };

private:
    static inline std::unordered_map<std::type_index, std::list<std::any>> handlers;
};

#include "messagebus.tpp"

#endif // MESSAGEBUS_HPP
