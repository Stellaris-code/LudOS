/*
interface.hpp

Copyright (c) 09 Yann BOUCHER (yann)

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
#ifndef INTERFACE_HPP
#define INTERFACE_HPP

#include "vfs.hpp"

namespace vfs
{

template <typename T, int id>
struct ientry
{
    using interface_type = T;
    static constexpr auto interface_id = id;
};

template <typename Derived, typename... T>
struct interface_node : public vfs::node
{
    static_assert (sizeof...(T) != 0, "an interface_node must have at least one interface");

    template<typename Interface>
    void fill_interface(Interface* interface) const
    {
        (void)interface;
    }

    virtual bool implements(int interface_id) const override
    {
        if constexpr (sizeof...(T) == 0) return false;
        else
        return implements_impl<T...>(interface_id);
    }

    virtual int get_interface(int interface_id, void* interface) const override
    {
        if constexpr (sizeof...(T) == 0) return -1;
        else
        return get_interface_impl<T...>(interface_id, interface);
    }

private:
    template <typename First, typename Second, typename... Rest>
    int get_interface_impl(int interface_id, void* interface) const
    {
        if (int ret = get_interface_impl<First>(interface_id, interface); ret != -1)
            return ret;
        return get_interface_impl<Second, Rest...>(interface);
    }

    template <typename First>
    int get_interface_impl(int interface_id, void* interface) const
    {
        if (First::interface_id != interface_id)
            return -1;

        memset(interface, 0, sizeof(typename First::interface_type));
        static_cast<const Derived*>(this)->fill_interface((typename First::interface_type*)interface);

        return sizeof(typename First::interface_type);
    }

    template <typename First, typename Second, typename... Rest>
    bool implements_impl(int interface_id) const
    {
        return First::interface_id == interface_id ||
                implements_impl<Second, Rest...>(interface_id);
    }

    template <typename First>
    bool implements_impl(int interface_id) const
    {
        return First::interface_id == interface_id;
    }
};

}

#endif // INTERFACE_HPP
