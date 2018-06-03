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
#include <optional.hpp>

#include <types/typeid.hpp>

class MessageBus
{
public:
    struct Entry;
    using Handle = std::pair<kpp::type_id_t, std::list<Entry>::const_iterator>;

public:
    enum Priority
    {
        Normal,
        Last
    };

    template <typename T>
    static Handle register_handler(std::function<void(const T&)> handler, Priority prio = Normal);

    static void remove_handler(const Handle& handle);

    // Returns the number of listeners who received the message
    template <typename T>
    static size_t send(const T& event);

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

        kpp::optional<Handle> handle {};
    };

public:
    struct Entry
    {
        std::function<void(const void*)> handler;
        Priority priority;
    };

private:
    static inline std::unordered_map<kpp::type_id_t, std::list<Entry>> handlers;
};

#include "messagebus.tpp"

#endif // MESSAGEBUS_HPP
