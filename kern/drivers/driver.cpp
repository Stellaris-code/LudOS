/*
driver.cpp

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

#include "driver.hpp"

#include <stdint.h>
#include "utils/kmsgbus.hpp"

#include "utils/logging.hpp"

extern "C" int start_driver_ctors;
extern "C" int end_driver_ctors;

namespace driver::detail
{
DriverEntry drivers[max_drivers];
DriverEntry* driver_list_ptr = drivers;
}

void Driver::interface_init()
{
    using namespace driver::detail;

    for (DriverEntry* ptr = drivers; ptr < driver_list_ptr; ++ptr)
    {
        (*ptr)();
    }
}

ref_vector<Driver> Driver::list()
{
    ref_vector<Driver> vec;
    for (auto& val : m_drivers) vec.emplace_back(*val);
    return vec;
}

void Driver::add_driver(std::unique_ptr<Driver>&& driver)
{
    m_drivers.emplace_back(std::move(driver));
    kmsgbus.send(DriverLoadedEvent{*m_drivers.back()});
}
