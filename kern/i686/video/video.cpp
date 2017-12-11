/*
video.cpp

Copyright (c) 02 Yann BOUCHER (yann)

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

#include "graphics/video.hpp"

#include "utils/virt_machine_detect.hpp"
#include "utils/logging.hpp"
#include "vbe.hpp"
#include "terminal/terminal.hpp"
#include "i686/cpu/mtrr.hpp"
#include "i686/mem/paging.hpp"

#include "graphics/drawing/display_draw.hpp"

// TODO : init correctement le current_mode

namespace graphics
{

VideoMode vbe_to_video_mode(const vbe::ModeInfoBlock& info)
{
#define VIDEO_VBE3_GET(name, mode_name) \
    if (info.Lin##name) mode.mode_name = info.Lin##name; \
    else                          mode.mode_name = info.name;

    VideoMode mode;
    mode.width = info.XResolution;
    mode.height = info.YResolution;
    mode.depth = info.BitsPerPixel;
    VIDEO_VBE3_GET(BytesPerScanLine, bytes_per_line);
    VIDEO_VBE3_GET(RedMaskSize, red_mask_size);
    VIDEO_VBE3_GET(RedFieldPosition, red_field_pos);
    VIDEO_VBE3_GET(GreenMaskSize, green_mask_size);
    VIDEO_VBE3_GET(GreenFieldPosition, green_field_pos);
    VIDEO_VBE3_GET(BlueMaskSize, blue_mask_size);
    VIDEO_VBE3_GET(BlueFieldPosition, blue_field_pos);
    mode.type = info.MemoryModel == vbe::MemoryModel::Text ? VideoMode::Text : VideoMode::Graphics;
    mode.framebuffer_addr = (uintptr_t)Memory::mmap((void*)info.PhysBasePtr,
                                                    mode.bytes_per_line*mode.height,
                                                    Memory::Write|Memory::WriteThrough|Memory::Uncached);

    return mode;
}

VideoMode current_mode;
std::unique_ptr<Screen> scr { nullptr };

std::vector<VideoMode> list_video_modes()
{
    auto vbe = vbe::get_vbe_info();
    if (vbe)
    {
        log(Info, "VBE is enabled\n");
        auto vbe_modes = vbe::get_vbe_modes(vbe->VideoModePtr);

        std::vector<VideoMode> modes;
        for (const auto& vbe_mode : vbe_modes)
        {
            modes.push_back(vbe_to_video_mode(vbe_mode.info));
        }

        return modes;
    }
    else
    {
        return {};
    }
}

std::optional<VideoMode> change_mode(size_t width, size_t height, size_t depth)
{
    auto vbe_info = vbe::get_vbe_info();
    if (!vbe_info) return {};

    auto list = vbe::get_vbe_modes(vbe_info->VideoModePtr);
    if (list.empty()) return {};

    auto mode = vbe::closest_mode(list, width, height, depth);
    if (mode.info.XResolution != width || mode.info.YResolution != height ||
            mode.info.BitsPerPixel != depth)
    {
        log(Notice, "Target resolution %dx%dx%d not found, switching to %dx%dx%d\n",
            width, height, depth, mode.info.XResolution, mode.info.YResolution, mode.info.BitsPerPixel);
    }

    if (vbe::set_mode(mode.mode))
    {
        reset_term();

        if (mtrr::available() && mtrr::available_variable_ranges()>0 && mtrr::supports_write_combining())
        {
            mtrr::set_variable_mtrr(mode.info.PhysBasePtr,
                                    mode.info.BytesPerScanLine*mode.info.YResolution,
                                    mtrr::WC);
        }

        Paging::mark_as_used(mode.info.PhysBasePtr,
                             mode.info.BytesPerScanLine*mode.info.YResolution);

        if (current_mode.framebuffer_addr) Memory::unmap((void*)current_mode.framebuffer_addr, current_mode.bytes_per_line*current_mode.height);

        current_mode = vbe_to_video_mode(mode.info);

        scr = std::make_unique<Screen>(current_mode.width, current_mode.height);
        set_display_mode(current_mode);
        return current_mode;
    }
    else
    {
        return {};
    }
}

VideoMode current_video_mode()
{
    return current_mode;
}

Screen *screen()
{
    return scr.get();
}

}
