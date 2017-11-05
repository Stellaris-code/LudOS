/*
vbe.cpp

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

#include "vbe.hpp"

#include "x86emu_modesetting.hpp"

#include "utils/logging.hpp"
#include "utils/memutils.hpp"
#include "utils/mathutils.hpp"

#include "graphics/video.hpp"

namespace vbe
{

std::optional<VbeInfoBlock> detail_get_info(bool vbe2)
{
    memset(emu_mem.data() + 0x20000, 0, sizeof(VbeInfoBlock));
    VbeInfoBlock* block = reinterpret_cast<VbeInfoBlock*>(emu_mem.data() + 0x20000);
    if (vbe2)
    {
        strncpy(reinterpret_cast<char*>(block->VbeSignature), "VBE2", 4);
    }

    auto state = emuInt10h(0x4F00, 0, 0, 0, 0x2000);
    const auto& emu = state.cpu_state.emu;

    if (emu->x86.R_AL == VBE_RETURN_STATUS_SUPPORTED)
    {
        if (emu->x86.R_AH == VBE_RETURN_STATUS_SUCCESSFULL)
        {
            return *block;
        }
        else
        {
            log(Debug, "VBE Info call failed, code 0x%x\n", emu->x86.R_AH);
            return {};
        }
    }
    else
    {
        log(Debug, "VBE Info interrupt not supported\n");
        return {};
    }
}

uint16_t get_vbe_version()
{
    auto vbe1info = detail_get_info(false);
    if (vbe1info)
    {
        return vbe1info->VbeVersion;
    }
    else
    {
        return 0x0000;
    }
}

std::optional<VbeInfoBlock> get_vbe_info()
{
    uint16_t version = get_vbe_version();

    std::optional<VbeInfoBlock> info;

    if ((version >> 8) >= 2)
    {
        info = detail_get_info(true);
    }
    else
    {
        info = detail_get_info(false);
    }

    if (info)
    {
        log(Debug, "VBE Version : 0x%x\n", info->VbeVersion);
        log(Debug, "OEM Name : %s\n", read_address(info->OemString));
        log(Debug, "Capabilities : %b\n", info->Capabilities & 0xFF);
        log(Debug, "Video Mode List Ptr : %x:%x (0x%x)\n", info->VideoModePtr.Seg, info->VideoModePtr.Off, translate_address(info->VideoModePtr));
        log(Debug, "Video Memory Amount : %s\n", human_readable_size(info->TotalMemory*64*1024).c_str());
        log(Debug, "OEM Software Revision : 0x%x\n", info->OemSoftwareRev);
        log(Debug, "OEM Product Name : %s\n", read_address(info->OemProductName));
        log(Debug, "OEM Product Rev : %s\n", read_address(info->OemProductRev));

    }

    return info;
}

std::vector<VideoMode> get_vbe_modes(rmode_ptr mode_list_ptr)
{
    std::vector<VideoMode> mode_info;

    std::vector<uint16_t> mode_list;
    uint16_t* modes = reinterpret_cast<uint16_t*>(read_address(mode_list_ptr));
    while (*modes != 0xFFFF)
    {
        mode_list.emplace_back(*modes);
        ++modes;
    }
    x86_flag = true;
    for (auto mode : mode_list)
    {
        memset(emu_mem.data() + 0x20000, 0, sizeof(ModeInfoBlock));
        ModeInfoBlock* block = reinterpret_cast<ModeInfoBlock*>(emu_mem.data() + 0x20000);
        auto state = emuInt10h(0x4F01, 0, mode, 0, 0x2000);
        const auto& emu = state.cpu_state.emu;

        if (emu->x86.R_AL == VBE_RETURN_STATUS_SUPPORTED)
        {
            if (emu->x86.R_AH == VBE_RETURN_STATUS_SUCCESSFULL)
            {
                if (is_mode_supported(*block))
                {
                    mode_info.push_back({mode, *block});
                }
            }
            else
            {
                log(Debug, "VBE Mode Info call failed for mode 0x%x, code 0x%x\n", mode, emu->x86.R_AH);
            }
        }
        else
        {
            log(Debug, "VBE Mode Info interrupt not supported\n");
            return {};
        }
    }

    return mode_info;
}

// source : https://github.com/mallardtheduck/osdev/blob/master/src/modules/vga/vbe.cpp
bool is_mode_supported(const ModeInfoBlock &mode)
{
    //Modes not supported by the hardware cannot be used
    if(!(mode.ModeAttributes & Attribute::Supported)) return false;

    //Don't support monochrome modes (really only applies to text)
    if(!(mode.ModeAttributes & Attribute::Colour)) return false;
    //Support only 4bpp text modes and 8, 16, 24 and 32bpp graphics modes
    if(!(mode.ModeAttributes & Attribute::Graphics) && mode.MemoryModel == MemoryModel::Text){
        if(mode.BitsPerPixel != 4) return false;
        //Disable text modes for now
        return false;
    }else if(mode.BitsPerPixel != 8 && mode.BitsPerPixel != 16 && mode.BitsPerPixel != 24 && mode.BitsPerPixel != 32){
        return false;
    }

    if (mode.BytesPerScanLine*mode.YResolution > video::max_res_pixels)
    {
        return false;
    }

    //Only support linear framebuffers for non text modes
    if(!(mode.ModeAttributes & Attribute::LinearMode) && mode.MemoryModel != MemoryModel::Text) return false;
    //If mode is not text, confirm we have a framebuffer address
    if(mode.MemoryModel != MemoryModel::Text && !mode.PhysBasePtr) return false;
    //Only support Text, Packed (8bpp) and DirectColour memory models
    if(mode.MemoryModel != MemoryModel::Text && mode.MemoryModel != MemoryModel::Packed && mode.MemoryModel != MemoryModel::DirectColour) return false;
    return true;
}

VideoMode closest_mode(const std::vector<VideoMode> &modes, size_t width, size_t height, size_t depth)
{
    assert(!modes.empty());

    VideoMode best_mode = modes.front();
    size_t best_pix_diff = diff<size_t>(modes.front().info.XResolution*modes.front().info.YResolution, width * height);
    size_t best_bpp_diff = diff<size_t>(modes.front().info.BitsPerPixel, depth);

    for (auto mode : modes)
    {
        size_t pix_diff = diff<size_t>(mode.info.XResolution * mode.info.YResolution, width * height);
        size_t bpp_diff = diff<size_t>(mode.info.BitsPerPixel, depth);
        if (pix_diff < best_pix_diff ||
                (pix_diff == best_pix_diff && bpp_diff < best_bpp_diff))
        {
            best_pix_diff = pix_diff;
            best_bpp_diff = bpp_diff;
            best_mode = mode;
        }
    }

    return best_mode;
}

bool set_mode(uint16_t mode)
{
    mode |= UseLFB | ClearDisplay;

    auto state = emuInt10h(0x4F02, mode, 0, 0, 0);
    const auto& emu = state.cpu_state.emu;

    if (emu->x86.R_AL == VBE_RETURN_STATUS_SUPPORTED)
    {
        if (emu->x86.R_AH == VBE_RETURN_STATUS_SUCCESSFULL)
        {
            return true;
        }
        else
        {
            log(Debug, "VBE Mode 0x%x set call failed, code 0x%x\n", mode, emu->x86.R_AH);
            return false;
        }
    }
    else
    {
        log(Debug, "VBE Modesetting interrupt not supported\n");
        return false;
    }
}

}
