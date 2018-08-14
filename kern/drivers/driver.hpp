/*
driver.hpp

Copyright (c) 14 Yann BOUCHER (yann)

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
#ifndef DRIVER_HPP
#define DRIVER_HPP

#include <stdint.h>
#include <type_traits.hpp>
#include <vector.hpp>
#include <memory.hpp>

#include "utils/vecutils.hpp"

#include "kstring/kstrfwd.hpp"
#include "utils/string_enum.hpp"

STRING_ENUM(DriverType, Keyboard, Mouse, NIC, IDEController, Sound, Disk, Debug)

struct DriverLoadedEvent
{
    class Driver& drv;
};

class Driver
{
    public:
    virtual ~Driver() = default;

    static void interface_init();

    static ref_vector<Driver> list();

    static void add_driver(std::unique_ptr<Driver>&& driver);

    template <typename T>
    static ref_vector<T> get_drivers()
    {
        ref_vector<T> vec;
        for (const auto& driver : m_drivers)
        {
            T* ptr = dynamic_cast<T*>(driver.get());
            if (ptr) vec.emplace_back(*ptr);
        }

        return vec;
    }

    public:
    virtual kpp::string driver_name() const = 0;
    virtual DriverType  type() const = 0;

    private:
    static inline std::vector<std::unique_ptr<Driver>> m_drivers;
};

namespace driver::detail
{
using DriverEntry = void(*)();

constexpr size_t max_drivers { 0x2000 };

extern DriverEntry drivers[max_drivers];
extern DriverEntry* driver_list_ptr;
}

#define ADD_DRIVER(name) \
    __attribute__((constructor)) void _driver_init_##name() \
{ \
    static_assert(std::is_base_of_v<Driver, name>); \
    *driver::detail::driver_list_ptr++ = [] { \
    if (name::accept()) \
    Driver::add_driver(std::make_unique<name>()); \
    }; \
    }

#endif // DRIVER_HPP
