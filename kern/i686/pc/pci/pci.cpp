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

#include "utils/logging.hpp"

uint16_t pci::read_reg(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset)
{
    uint32_t address = (uint32_t)((bus << 16) | (slot << 11) |
                                  (func << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

    /* write out the address */
    outl(0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    auto tmp = ((inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    if (offset % 2)
    {
        tmp >>= 8;
    }
    return (tmp);
}

void pci::write_reg(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset, uint16_t val)
{
    uint32_t address = (uint32_t)((bus << 16) | (slot << 11) |
                                  (func << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

    /* write out the address */
    outl(0xCF8, address);

    outl(0xCFC, val);
}

uint16_t pci::device_id(uint16_t bus, uint16_t slot, uint16_t func)
{
    return read_reg(bus, slot, func, 2);
}

uint16_t pci::vendor_id(uint16_t bus, uint16_t slot, uint16_t func)
{
    return read_reg(bus, slot, func, 0);
}

uint8_t pci::header_type(uint16_t bus, uint16_t slot, uint16_t func)
{
    return read_reg(bus, slot, func, 14) & 0xFF;
}

void pci::check_device(uint8_t bus, uint8_t device)
{
    uint8_t function = 0;

    uint16_t vendorID = vendor_id(bus, device, function);
    if(vendorID == 0xFFFF) return;        // Device doesn't exist
    check_function(bus, device, function);
    uint8_t headerType = header_type(bus, device, function);
    if( (headerType & 0x80) != 0)
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
    log("pci device %d on bus %d, function %d exists\n", device, bus, function);
    log("   Vendor : '%s'\n", vendor_string(vendor_id(bus, device, function)).c_str());
    log("   Device : '%s'\n", dev_string(vendor_id(bus, device, function), device_id(bus, device, function)).c_str());
    log("   Class : '%s'\n", class_code_string(base_class(bus, device, function), sub_class(bus, device, function), prog_if(bus, device, function)).c_str());
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
    return read_reg(bus, slot, func, 11);
}

uint8_t pci::sub_class(uint16_t bus, uint16_t slot, uint16_t func)
{
    return read_reg(bus, slot, func, 10);
}


uint8_t pci::prog_if(uint16_t bus, uint16_t slot, uint16_t func)
{
    return read_reg(bus, slot, func, 9);
}
