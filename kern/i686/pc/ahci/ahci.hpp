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

#include "i686/cpu/registers.hpp"

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
    volatile uint32_t	ci;		// 0x38, command issue
    uint32_t	sntf;		// 0x3C, SATA notification (SCR4:SNotification)
    uint32_t	fbs;		// 0x40, FIS-based switch control
    uint32_t	rsv1[11];	// 0x44 ~ 0x6F, Reserved
    uint32_t	vendor[4];	// 0x70 ~ 0x7F, vendor specific
};

enum class HBAPortDet
{
    NoDevice = 0,
    DeviceNoPhy = 1,
    DevicePresent = 3,
    Disabled = 4
};

enum class HBAPortIpm
{
    NoDevice = 0,
    Active = 1,
    Partial = 2,
    Slumber = 6
};

enum class SATASig : uint32_t
{
    SATA = 0x00000101,
    SATAPI = 0xEB140101,
    SEMB = 0xC33C0101,
    PM = 0x96690101
};

struct [[gnu::packed]] HBAMem
{
    // 0x00 - 0x2B, Generic Host Control
    union
    {
        const uint32_t	cap;		// 0x00, Host capability
        struct
        {
            uint32_t np : 4;
            uint32_t unus0 : 4;
            uint32_t ncs : 4;
            uint32_t unus1 : 18;
            uint32_t sncq : 1;
            uint32_t s64a : 1;
        };
    };
    uint32_t ghc;
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
    uint8_t	rsv[116];

    // 0xA0 - 0xFF, Vendor specific registers
    uint8_t	vendor[96];

    // 0x100 - 0x10FF, Port control registers
    volatile HBAPort	ports[];	// 1 ~ 32
};
static_assert(sizeof(HBAMem) == 256);

struct alignas(256) [[gnu::packed]] ReceivedFIS
{
    uint8_t dma_setup_fis[28];

    uint8_t resv0[4];

    uint8_t pio_setup_fis[20];

    uint8_t resv1[12];

    uint8_t d2h_register_fis[20];

    uint8_t resv2[4];

    uint8_t sdbfis[8];

    uint8_t unknown_fis[64];

    uint8_t resv3[96];
};
static_assert(sizeof(ReceivedFIS) == 256);

enum class FISType : uint8_t
{
    RegH2D	= 0x27,	// Register FIS - host to device
    RegD2H	= 0x34,	// Register FIS - device to host
    DMAAct	= 0x39,	// DMA activate FIS - device to host
    DMASetup	= 0x41,	// DMA setup FIS - bidirectional
    Data		= 0x46,	// Data FIS - bidirectional
    BIST		= 0x58,	// BIST activate FIS - bidirectional
    PIOStetup	= 0x5F,	// PIO setup FIS - device to host
    DevBits	= 0xA1,	// Set device bits FIS - device to host
};

struct [[gnu::packed]] FisRegH2D
{
    // DWORD 0
    FISType	fis_type;	// FIS_TYPE_REG_H2D

    uint8_t	pmport:4;	// Port multiplier
    uint8_t	rsv0:3;		// Reserved
    uint8_t	c:1;		// 1: Command, 0: Control

    uint8_t	command;	// Command register
    uint8_t	featurel;	// Feature register, 7:0

    // DWORD 1
    uint8_t	lba0;		// LBA low register, 7:0
    uint8_t	lba1;		// LBA mid register, 15:8
    uint8_t	lba2;		// LBA high register, 23:16
    uint8_t	device;		// Device register

    // DWORD 2
    uint8_t	lba3;		// LBA register, 31:24
    uint8_t	lba4;		// LBA register, 39:32
    uint8_t	lba5;		// LBA register, 47:40
    uint8_t	featureh;	// Feature register, 15:8

    // DWORD 3
    uint8_t	countl;		// Count register, 7:0
    uint8_t	counth;		// Count register, 15:8
    uint8_t	icc;		// Isochronous command completion
    uint8_t	control;	// Control register

    // DWORD 4
    uint8_t	rsv1[4];	// Reserved
};
static_assert(sizeof(FisRegH2D) == 20);

struct [[gnu::packed]] PrdtEntry
{
    uint32_t	dba;		// Data base address
    uint32_t	dbau;		// Data base address upper 32 bits
    uint32_t	rsv0;		// Reserved

    // DW3
    uint32_t	dbc:22;		// Byte count, 4M max
    uint32_t	rsv1:9;		// Reserved
    uint32_t	i:1;		// Interrupt on completion
};
static_assert(sizeof(PrdtEntry) == 16);

struct [[gnu::packed]] CommandHeader
{
    uint32_t cfl : 5;
    uint32_t atapi : 1;
    uint32_t write : 1;
    uint32_t prefetchable : 1;
    uint32_t reset : 1;
    uint32_t BIST : 1;
    uint32_t clear_busy : 1;
    uint32_t resv0 : 1;
    uint32_t pmp : 4;
    uint32_t prdtl : 16;

    uint32_t status;

    uint32_t ctba;

    uint32_t ctbau;

    uint32_t resv2[4];
};
static_assert(sizeof(CommandHeader) == 32);

struct alignas(1024) [[gnu::packed]] CommandList
{
    CommandHeader hdrs[32];
};
static_assert(sizeof(CommandList) == 1024);

struct alignas(128) [[gnu::packed]] CommandTable
{
    uint8_t command_fis[64];

    uint8_t atapi_command[16]; // 12 or 16

    uint8_t resv[48];

    PrdtEntry entries[];
};

static constexpr uint32_t cap_s64a = 1<<31;
static constexpr uint32_t cap_sncq = 1<<30;

static constexpr uint32_t ghd_ahci_enable = 1<<31;
static constexpr uint32_t ghd_int_enable  = 1<<1;

static constexpr uint32_t bohc_bios_busy      = 1<<4;
static constexpr uint32_t bohc_os_ownership   = 1<<1;
static constexpr uint32_t bohc_bios_ownership = 1<<0;

static constexpr uint32_t pxcmd_st = 1<<0;
static constexpr uint32_t pxcmd_fre= 1<<4;
static constexpr uint32_t pxcmd_fr = 1<<14;
static constexpr uint32_t pxcmd_cr = 1<<15;

static constexpr uint32_t sctl_det = 1<<0;
static constexpr uint32_t ssts_det = 1<<0;

static constexpr uint32_t int_dma_setup = 1<<2;
static constexpr uint32_t int_dhr_setup = 1<<0;

static constexpr uint32_t pxis_tfes = 1<<30;
static constexpr uint32_t pxis_hbfs = 1<<29;
static constexpr uint32_t pxis_hbds = 1<<28;
static constexpr uint32_t pxis_ifs = 1<<27;
static constexpr uint32_t pxis_ifns = 1<<26;
static constexpr uint32_t pxis_ofs = 1<<24;
static constexpr uint32_t pxis_dps = 1<<5;

static constexpr uint32_t ata_read_dma_ex = 0x25;
static constexpr uint32_t ata_write_dma_ex = 0x35;
static constexpr uint32_t ata_identify = 0xEC;

static constexpr uint32_t ata_busy = 1<<7;
static constexpr uint32_t ata_drq = 1<<3;

enum class PortType
{
    SATA,
    SATAPI,
    SEMB,
    PM,
    Null
};

void ahci_isr(const registers* reg);

HBAMem* get_hbamem_ptr();

uint8_t get_interrupt_line();

void get_ahci_ownership();

void mkprd(PrdtEntry& entry, uint64_t addr, size_t bytes);
[[nodiscard]] bool issue_read_command(size_t port, uint64_t sector, size_t count, uint16_t* buf);
[[nodiscard]] bool issue_write_command(size_t port, uint64_t sector, size_t count, const uint16_t* buf);

uint32_t flush_commands(size_t port);

void init_interface();

PortType get_port_type(size_t port);
void init_port(size_t port);
void reset_port(size_t port);
void clear_errs(size_t port);
void init_memory(size_t port);
void init_port_interrupts(size_t port);
void stop_port(size_t port);
void start_port(size_t port);
int free_slot(size_t port);
bool check_errors(size_t port);

}

extern volatile detail::HBAMem* volatile mem;

extern detail::CommandList cmdlists[32];
extern detail::ReceivedFIS rcvfis[32];
extern detail::CommandTable cmdtables[32];

}

#endif // AHCI_HPP
