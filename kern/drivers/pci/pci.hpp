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

#include <vector.hpp>

#include "io.hpp"

namespace pci
{

struct [[gnu::packed]] PciDevice
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

    union
    {
        struct
        {
            uint32_t bar[6];
            uint32_t* cardbus_cis;
            uint16_t subsys_venid;
            uint16_t subsys_id;
            uint32_t* expansion_rom_addr;
            uint8_t capabilities;
            uint32_t reserved0 : 24;
            uint32_t reserved1;
            uint8_t int_line;
            uint8_t int_pin;
            uint8_t min_grant;
            uint8_t max_latency;
        };
    };

    uint16_t bus;
    uint16_t slot;
    uint16_t func;
};


enum class BARType
{
    Mem16,
    Mem32,
    Mem64,
    IO16,
    Invalid = 0xFF
};

inline BARType bar_type(uint32_t bar)
{
    if (bar & 1)
    {
        return BARType::IO16;
    }
    if ((bar & 0b110) == 0)
    {
        return BARType::Mem32;
    }
    if ((bar & 0b110) == 1)
    {
        return BARType::Mem16;
    }
    if ((bar & 0b110) == 2)
    {
        return BARType::Mem64;
    }

    return BARType::Invalid;
}

uint64_t get_bar_val(const pci::PciDevice& dev, size_t bar_idx);

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

PciDevice get_dev(uint16_t bus, uint16_t slot, uint16_t func);

void scan();

std::vector<PciDevice> find_devices(uint8_t class_code, uint8_t sub_class);
std::vector<PciDevice> find_devices(uint8_t class_code, uint8_t sub_class, uint8_t interface);

extern std::vector<PciDevice> devices;
}

#endif // PCI_HPP
