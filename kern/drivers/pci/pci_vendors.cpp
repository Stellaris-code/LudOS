/*
pci_vendors.cpp

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

#include "pci_vendors.hpp"

#include "external/pci_vendors.h"

#include <stdio.h>

#include <kstring/kstring.hpp>

namespace pci
{

kpp::string class_code_string(uint16_t base_class, uint16_t sub_class, uint16_t prog_if)
{
    for (const auto& el : PciClassCodeTable)
    {
        if (el.BaseClass == base_class && el.SubClass == sub_class && (el.ProgIf == 0xFF || el.ProgIf == prog_if))
        {
            return kpp::string(el.BaseDesc) + ":" + el.SubDesc + ":" + el.ProgDesc;
        }
    }

    char buf[256];
    ksnprintf(buf, 256, "Unknown (0x%x:0x%x:0x%x)", base_class, sub_class, prog_if);
    return buf;
}

kpp::string dev_string(uint16_t ven_id, uint16_t dev_id)
{
    for (const auto& el : PciDevTable)
    {
        if (el.VenId == ven_id && el.DevId == dev_id)
        {
            return kpp::string(el.Chip) + " " + el.ChipDesc;
        }
    }

    return "Unknown";
}

kpp::string vendor_string(uint16_t ven_id)
{
    for (const auto& el : PciVenTable)
    {
        if (el.VenId == ven_id)
        {
            return kpp::string(el.VenFull) + "(" + el.VenShort + ")";
        }
    }

    return "Unknown";
}

}
