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

struct PciDevice
{
    uint16_t vendorID;
    uint16_t deviceID;
    uint16_t command;
    uint16_t status;
    uint8_t revID;
    uint8_t progIF;
    uint8_t subclass;
    uint8_t classCode;
    uint8_t cacheLineSize;
    uint8_t latencyTimer;
    uint8_t headerType;
    uint8_t BIST;
};

uint16_t read_reg(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset);
void write_reg(uint16_t bus, uint16_t slot, uint16_t func, uint16_t offset, uint16_t val);

uint16_t device_id(uint16_t bus, uint16_t slot, uint16_t func);
uint16_t vendor_id(uint16_t bus, uint16_t slot, uint16_t func);
uint8_t header_type(uint16_t bus, uint16_t slot, uint16_t func);
uint8_t base_class(uint16_t bus, uint16_t slot, uint16_t func);
uint8_t sub_class(uint16_t bus, uint16_t slot, uint16_t func);
uint8_t prog_if(uint16_t bus, uint16_t slot, uint16_t func);

void check_device(uint8_t bus, uint8_t device);
void check_function(uint8_t bus, uint8_t device, uint8_t function);

void scan();
}

#endif // PCI_HPP
