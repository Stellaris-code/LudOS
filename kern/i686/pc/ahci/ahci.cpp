/*
ahci.cpp

Copyright (c) 07 Yann BOUCHER (yann)

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

#include "ahci.hpp"

#include "../pci/pci.hpp"

#include "utils/logging.hpp"
#include "utils/bitops.hpp"

namespace ahci
{

volatile detail::HBAMem* volatile mem;

bool available()
{
    return !pci::find_devices(0x1, 0x6, 0x1).empty();
}

bool init()
{
    mem = detail::get_hbamem_ptr();
    if (!mem)
    {
        err("No valid AHCI controller found, aborting !\n");
        return false;
    }
    auto ghc = mem->ghc;
    ghc |= detail::ghd_ahci_enable | detail::ghd_int_enable;
    mem->ghc = ghc;

    log("AHCI version : %d%d.%d%d\n", (mem->major_vs>>8)&0xFF, mem->major_vs&0xFF, (mem->minor_vs>>8)&0xFF, mem->minor_vs&0xFF);
    log("AHCI capabilities : %b\n", mem->cap);

    log("Available AHCI ports : \n");
    for (size_t i { 0 }; i < sizeof(mem->pi)*CHAR_BIT; ++i)
    {
        if (bit_check(mem->pi, i))
        {
            log("   Port %d\n", i);
        }
    }

    return false;
}

detail::HBAMem *detail::get_hbamem_ptr()
{
    if (!available()) return nullptr;

    auto ahci_con = pci::find_devices(0x1, 0x6, 0x1)[0];

    return reinterpret_cast<HBAMem*>(pci::get_bar_val(ahci_con, 5));
}

}
