/*
ahci.hpp

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
#ifndef AHCI_HPP
#define AHCI_HPP

#include <stdint.h>

namespace ahci
{
bool available();

bool init();

namespace detail
{
struct [[gnu::packed]] HBAPort
{
    uint32_t	clb;		// 0x00, command list base address, 1K-byte aligned
    uint32_t	clbu;		// 0x04, command list base address upper 32 bits
    uint32_t	fb;		// 0x08, FIS base address, 256-byte aligned
    uint32_t	fbu;		// 0x0C, FIS base address upper 32 bits
    uint32_t	is;		// 0x10, interrupt status
    uint32_t	ie;		// 0x14, interrupt enable
    uint32_t	cmd;		// 0x18, command and status
    uint32_t	rsv0;		// 0x1C, Reserved
    uint32_t	tfd;		// 0x20, task file data
    uint32_t	sig;		// 0x24, signature
    uint32_t	ssts;		// 0x28, SATA status (SCR0:SStatus)
    uint32_t	sctl;		// 0x2C, SATA control (SCR2:SControl)
    uint32_t	serr;		// 0x30, SATA error (SCR1:SError)
    uint32_t	sact;		// 0x34, SATA active (SCR3:SActive)
    uint32_t	ci;		// 0x38, command issue
    uint32_t	sntf;		// 0x3C, SATA notification (SCR4:SNotification)
    uint32_t	fbs;		// 0x40, FIS-based switch control
    uint32_t	rsv1[11];	// 0x44 ~ 0x6F, Reserved
    uint32_t	vendor[4];	// 0x70 ~ 0x7F, vendor specific
};

struct [[gnu::packed]] HBAMem
{
    // 0x00 - 0x2B, Generic Host Control
    const uint32_t	cap;		// 0x00, Host capability
    union
    {
        struct
        {
            const uint32_t    hba_reset : 1;
            const uint32_t    int_enable : 1;
            const uint32_t    mrsm : 1;
            const uint32_t    resv : 28;
            const uint32_t    ahci_enable : 1;
        };
        uint32_t ghc;
    };
    uint32_t	is;		// 0x08, Interrupt status
    const uint32_t	pi;		// 0x0C, Port implemented
    const uint32_t	minor_vs : 16;		// 0x10, Version minor
    const uint32_t	major_vs : 16;		// 0x10, Version major
    uint32_t	ccc_ctl;	// 0x14, Command completion coalescing control
    uint32_t	ccc_pts;	// 0x18, Command completion coalescing ports
    uint32_t	em_loc;		// 0x1C, Enclosure management location
    uint32_t	em_ctl;		// 0x20, Enclosure management control
    uint32_t	cap2;		// 0x24, Host capabilities extended
    uint32_t	bohc;		// 0x28, BIOS/OS handoff control and status

    // 0x2C - 0x9F, Reserved
    uint8_t	rsv[0xA0-0x2C];

    // 0xA0 - 0xFF, Vendor specific registers
    uint8_t	vendor[0x100-0xA0];

    // 0x100 - 0x10FF, Port control registers
    HBAPort	ports[];	// 1 ~ 32
};

static constexpr uint32_t ghd_ahci_enable = 1<<31;
static constexpr uint32_t ghd_int_enable = 1<<1;

static_assert(sizeof(HBAMem) == 256);

HBAMem* get_hbamem_ptr();

}
}

#endif // AHCI_HPP
