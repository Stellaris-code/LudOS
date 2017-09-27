/*
pci.hpp

Copyright (c) 20 Yann BOUCHER (yann)

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
#ifndef PCI_HPP
#define PCI_HPP

#include <stdint.h>

#include "io.hpp"

namespace pci
{
uint16_t read_reg(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset)
{
    uint32_t address = (uint32_t)((bus << 16) | (slot << 11) |
              (func << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

    /* write out the address */
    outl(0xCF8, address);
    /* read in the data */
    /* (offset & 2) * 8) = 0 will choose the first word of the 32 bits register */
    auto tmp = ((inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return (tmp);
}

void write_reg(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset, uint16_t val)
{
    uint32_t address = (uint32_t)((bus << 16) | (slot << 11) |
              (func << 8) | (offset & 0xfc) | ((uint32_t)0x80000000));

    /* write out the address */
    outl(0xCF8, address);

    outl(0xCFC, val);
}
}

#endif // PCI_HPP
