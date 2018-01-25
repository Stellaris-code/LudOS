/*
pcidriver.hpp

Copyright (c) 13 Yann BOUCHER (yann)

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
#ifndef PCIDRIVER_HPP
#define PCIDRIVER_HPP

#include "drivers/driver.hpp"

#include <stdint.h>

#include "pci.hpp"

class PciDriver : public Driver
{
public:
    static void interface_init();

    static bool accept(const pci::PciDevice& dev) { return false; }

    void set_pci_info(const pci::PciDevice& dev) { m_dev = dev; }

    virtual void init() = 0;

protected:
    pci::PciDevice m_dev;
};

namespace pci::detail
{
    using PciDriverEntry = void(*)(const pci::PciDevice& dev);

    constexpr size_t max_drivers { 0x2000 };

    extern PciDriverEntry drivers[max_drivers];
    extern PciDriverEntry* driver_list_ptr;
}

#define ADD_PCI_DRIVER(name) \
__attribute__((constructor)) void _pci_init_##name() \
{ \
    *pci::detail::driver_list_ptr++ = [](const pci::PciDevice& dev) \
    { \
        if (name::accept(dev)) \
        { \
            name* obj = new name; /* notice the absence of free, we don't support PnP so it does'nt really makes sense to support driver removal */ \
            obj->set_pci_info(dev); \
            obj->init(); \
        } \
    }; \
}

#endif // PCIDRIVER_HPP
