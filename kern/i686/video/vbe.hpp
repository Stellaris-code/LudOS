/*
vbe.hpp

Copyright (c) 31 Yann BOUCHER (yann)

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
#ifndef VBE_HPP
#define VBE_HPP

#include <stdint.h>

#include <optional.hpp>

#include "x86emu_modesetting.hpp"

// DISPI helper function
//void dispi_set_enable(Boolean enable);

namespace vbe
{

// The official VBE Information Block
struct [[gnu::packed]] VbeInfoBlock
{
    char      VbeSignature[4];
    uint16_t     VbeVersion;
    rmode_ptr      OemString;
    uint32_t      Capabilities;
    rmode_ptr      VideoModePtr;
    uint16_t     TotalMemory;
    uint16_t     OemSoftwareRev;
    rmode_ptr      OemVendorName;
    rmode_ptr      OemProductName;
    rmode_ptr      OemProductRev;
    uint16_t     Reserved[111]; // used for dynamically generated mode list
    uint8_t      OemData[256];
} __attribute__((packed));
static_assert(sizeof(VbeInfoBlock) == 512);

struct [[gnu::packed]] ModeInfoBlock
{
    // Mandatory information for all VBE revisions
    uint16_t ModeAttributes;
    uint8_t  WinAAttributes;
    uint8_t  WinBAttributes;
    uint16_t WinGranularity;
    uint16_t WinSize;
    uint16_t WinASegment;
    uint16_t WinBSegment;
    uint32_t WinFuncPtr;
    uint16_t BytesPerScanLine;
    // Mandatory information for VBE 1.2 and above
    uint16_t XResolution;
    uint16_t YResolution;
    uint8_t  XCharSize;
    uint8_t  YCharSize;
    uint8_t  NumberOfPlanes;
    uint8_t  BitsPerPixel;
    uint8_t  NumberOfBanks;
    uint8_t  MemoryModel;
    uint8_t  BankSize;
    uint8_t  NumberOfImagePages;
    uint8_t  Reserved_page;
    // Direct Color fields (required for direct/6 and YUV/7 memory models)
    uint8_t  RedMaskSize;
    uint8_t  RedFieldPosition;
    uint8_t  GreenMaskSize;
    uint8_t  GreenFieldPosition;
    uint8_t  BlueMaskSize;
    uint8_t  BlueFieldPosition;
    uint8_t  RsvdMaskSize;
    uint8_t  RsvdFieldPosition;
    uint8_t  DirectColorModeInfo;
    // Mandatory information for VBE 2.0 and above
    uint32_t PhysBasePtr;
    uint32_t OffScreenMemOffset;
    uint16_t OffScreenMemSize;
    // Mandatory information for VBE 3.0 and above
    uint16_t LinBytesPerScanLine;
    uint8_t  BnkNumberOfPages;
    uint8_t  LinNumberOfPages;
    uint8_t  LinRedMaskSize;
    uint8_t  LinRedFieldPosition;
    uint8_t  LinGreenMaskSize;
    uint8_t  LinGreenFieldPosition;
    uint8_t  LinBlueMaskSize;
    uint8_t  LinBlueFieldPosition;
    uint8_t  LinRsvdMaskSize;
    uint8_t  LinRsvdFieldPosition;
    uint32_t MaxPixelClock;
    uint8_t  Reserved[190];
} __attribute__((packed));
static_assert(sizeof(ModeInfoBlock) == 256);

// VBE Return Status Info
// AL
#define VBE_RETURN_STATUS_SUPPORTED                      0x4F
#define VBE_RETURN_STATUS_UNSUPPORTED                    0x00
// AH
#define VBE_RETURN_STATUS_SUCCESSFULL                    0x00
#define VBE_RETURN_STATUS_FAILED                         0x01
#define VBE_RETURN_STATUS_NOT_SUPPORTED                  0x02
#define VBE_RETURN_STATUS_INVALID                        0x03

enum Attribute : uint16_t
{
    Supported = (1 << 0),
    Reserved0 = (1 << 1),
    BiosTTY = (1 << 2),
    Colour = (1 << 3),
    Graphics = (1 << 4),
    NonVGA = (1 << 5),
    VGAWinMode = (1 << 6),
    LinearMode = (1 << 7)
};

enum MemoryModel : uint8_t
{
    Text = 0x00,
    CGA = 0x01,
    Hercules = 0x02,
    Planar = 0x03,
    Packed = 0x04,
    NonChain = 0x05,
    DirectColour = 0x06,
    YUV = 0x07
};

enum Modebits : uint16_t
{
    UseLFB = (1<<14),
    ClearDisplay = (1<<15)
};

struct VideoMode
{
    uint16_t mode;
    ModeInfoBlock info;
};

uint16_t get_vbe_version(); // 0 if absent

std::optional<VbeInfoBlock> get_vbe_info();
std::vector<VideoMode> get_vbe_modes(rmode_ptr mode_list);
bool is_mode_supported(const ModeInfoBlock &mode);
VideoMode closest_mode(const std::vector<VideoMode>& modes, size_t width, size_t height, size_t depth);

bool set_mode(uint16_t mode);

}

#endif // VBE_HPP
