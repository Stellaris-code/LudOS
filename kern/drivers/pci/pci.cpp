/*
pci.cpp

Copyright (c) 05 Yann BOUCHER (yann)

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

#include "pci.hpp"

#include "pci_vendors.hpp"

#include "io.hpp"

#include "utils/logging.hpp"

std::vector<pci::PciDevice> pci::devices;

inline uint32_t devaddr(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset)
{
    return (uint32_t)(bus  << 16) | (uint32_t)(slot   << 11 ) |
           (uint32_t)(func << 8 ) | (uint32_t)(offset & ~0x3) | 0x80000000u;
}

uint32_t pci::read32(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset)
{
    assert((offset & 0x3) == 0); // only aligned accesses

    outl(0xCF8, devaddr(bus, slot, func, offset));

    return inl(0xCFC);
}

uint16_t pci::read16(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset)
{
    assert((offset % 0x1) == 0); // only aligned accesses

    uint32_t val = read32(bus, slot, func, offset & ~0x3);
    if (offset & 0x3) return val & 0xFFFF;
    else              return val >> 16;
}

uint8_t pci::read8(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset)
{
    uint32_t val = read32(bus, slot, func, offset & ~0x3);
    return (val >> 8*(offset&0x3)) & 0xFF;
}

void pci::write32(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset, uint32_t val)
{
    assert((offset & 0x3) == 0);

    outl(0xCF8, devaddr(bus, slot, func, offset));

    outl(0xCFC, val);
}

void pci::write16(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset, uint16_t val)
{
    assert((offset & 0x1) == 0);

    uint32_t src_val = read32(bus, slot, func, offset & ~0x3);
    uint32_t mask    = (0x0000FFFF << 8*(offset&0x3));
    src_val &= mask;
    src_val |= (val << (16 - 8*(offset&0x3)));

    write32(bus, slot, func, offset & ~0x3, src_val);
}

void pci::write8(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset, uint8_t val)
{
    assert((offset & 0x1) == 0);

    uint32_t src_val = read32(bus, slot, func, offset & ~0x3);
    uint32_t mask    = (0x000000FF << 8*(offset&0x3));
    src_val &= mask;
    src_val |= (val << (32 - 8*(offset&0x3)));

    write32(bus, slot, func, offset & ~0x3, src_val);
}


uint16_t pci::device_id(uint16_t bus, uint16_t slot, uint16_t func)
{
    return read16(bus, slot, func, 0);
}

uint16_t pci::vendor_id(uint16_t bus, uint16_t slot, uint16_t func)
{
    return read16(bus, slot, func, 2);
}

uint8_t pci::header_type(uint16_t bus, uint16_t slot, uint16_t func)
{
    return read8(bus, slot, func, 0xD);
}

void pci::check_device(uint8_t bus, uint8_t device)
{
    uint8_t function = 0;

    uint16_t vendorID = vendor_id(bus, device, function);
    if(vendorID == 0xFFFF) return;        // Device doesn't exist
    check_function(bus, device, function);
    uint8_t headerType = header_type(bus, device, function);
    if( (headerType & 0x80) != 0 || true)
    {
        /* It is a multi-function device, so check remaining functions */
        for(function = 1; function < 8; function++)
        {
            if(vendor_id(bus, device, function) != 0xFFFF)
            {
                check_function(bus, device, function);
            }
        }
    }
}

void pci::check_function(uint8_t bus, uint8_t device, uint8_t function)
{
    auto dev = get_dev(bus, device, function);

    pci::devices.emplace_back(dev);
}

void pci::scan()
{
    for(uint16_t bus = 0; bus < 256; bus++)
    {
        for (uint8_t device = 0; device < 32; device++)
        {
            check_device(bus, device);
        }
    }
}

uint8_t pci::base_class(uint16_t bus, uint16_t slot, uint16_t func)
{
    return read8(bus, slot, func, 0x8);
}

uint8_t pci::sub_class(uint16_t bus, uint16_t slot, uint16_t func)
{
    return read8(bus, slot, func, 0x9);
}

uint8_t pci::prog_if(uint16_t bus, uint16_t slot, uint16_t func)
{
    return read8(bus, slot, func, 0xA);
}

pci::PciDevice pci::get_dev(uint16_t bus, uint16_t slot, uint16_t func)
{
    PciDevice dev;
    for (size_t i { 0 }; i < sizeof(dev) / sizeof(uint32_t); ++i)
    {
        uint32_t dword = read32(bus, slot, func, i*sizeof(uint32_t));

        reinterpret_cast<uint32_t*>(&dev)[i] = dword;
    }

    dev.bus = bus;
    dev.slot = slot;
    dev.func = func;

    return dev;
}

std::vector<pci::PciDevice> pci::find_devices(uint8_t class_code, uint8_t sub_class)
{
    std::vector<pci::PciDevice> devices;

    for (const auto& device : pci::devices)
    {
        if (device.classCode == class_code && device.subclass == sub_class)
        {
            devices.emplace_back(device);
        }
    }

    return devices;
}

std::vector<pci::PciDevice> pci::find_devices(uint8_t class_code, uint8_t sub_class, uint8_t interface)
{
    std::vector<pci::PciDevice> devices;

    for (const auto& device : pci::devices)
    {
        if (device.classCode == class_code && device.subclass == sub_class && device.progIF == interface)
        {
            devices.emplace_back(device);
        }
    }

    return devices;
}

uint64_t pci::get_bar_val(const pci::PciDevice& dev, size_t bar_idx)
{
    assert(bar_idx <= 6);

    auto type = bar_type(dev.bar[bar_idx]);

    switch (type)
    {
        case BARType::IO16:
            return dev.bar[bar_idx] & 0xFFFFFFFC;
        case BARType::Mem16:
            return dev.bar[bar_idx] & 0xFFF0;
        case BARType::Mem32:
            return dev.bar[bar_idx] & 0xFFFFFFF0;
        case BARType::Mem64:
            return (dev.bar[bar_idx] & 0xFFFFFFF0) + (uint64_t(dev.bar[bar_idx+1] & 0xFFFFFFFF) << 32);
        case BARType::Invalid:
            assert(false);
    }

    return 0;
}

uint8_t get_irq(const pci::PciDevice &dev)
{
    // TODO : IO APIC
    return dev.int_line;
}
