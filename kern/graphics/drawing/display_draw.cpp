/*
display_draw.cpp

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

#include "display_draw.hpp"

#include "utils/logging.hpp"
#include "io.hpp"

namespace graphics
{

inline void set_pixel_field(uint32_t* src, uint32_t dst, int pos, int len)
{
    uint32_t mask = (1u << len) - 1;
    uint32_t shifted_mask = mask << pos;
    *src = ((dst & mask) << pos) | (*src & ~shifted_mask);
}

void draw_to_display_naive(const Screen &screen)
{
    assert(screen.width()*screen.height() >= current_video_mode().height*current_video_mode().width);

    __builtin_prefetch(screen.data(), 0, 0);

    for (size_t j { 0 }; j < current_video_mode().height; ++j)
    {
        for (size_t i { 0 }; i < current_video_mode().width; ++i)
        {
            auto color = screen[{i, j}];
            const size_t offset = i * current_video_mode().depth/CHAR_BIT + j * current_video_mode().bytes_per_line;
            uint32_t* pixel = reinterpret_cast<uint32_t*>(current_video_mode().framebuffer_addr + offset);
            set_pixel_field(pixel, color.r, current_video_mode().red_field_pos, current_video_mode().red_mask_size);
            set_pixel_field(pixel, color.g, current_video_mode().green_field_pos, current_video_mode().green_mask_size);
            set_pixel_field(pixel, color.b, current_video_mode().blue_field_pos, current_video_mode().blue_mask_size);
        }
    }
}

void draw_to_display_32rgb_nopad(const Screen &screen)
{    
    const auto mode = current_video_mode();

    assert(screen.width()*screen.height() >= mode.height*mode.width);

    __builtin_prefetch(screen.data(), 0, 0);

    aligned_memcpy(reinterpret_cast<void*>(mode.framebuffer_addr), screen.data(),
                   mode.width*mode.height*mode.depth/CHAR_BIT);
}

void clear_display(Color color)
{
#if 0

    uint32_t value = 0;
    set_pixel_field(&value, color.r, current_video_mode().red_field_pos, current_video_mode().red_mask_size);
    set_pixel_field(&value, color.g, current_video_mode().green_field_pos, current_video_mode().green_mask_size);
    set_pixel_field(&value, color.b, current_video_mode().blue_field_pos, current_video_mode().blue_mask_size);

    aligned_memsetl(reinterpret_cast<void*>(current_video_mode().framebuffer_addr), value, current_video_mode().width*current_video_mode().height*current_video_mode().depth/CHAR_BIT);
#else
    Screen screen(current_video_mode().width, current_video_mode().height);
    memsetl(screen.data(), color.rgb(), screen.width()*screen.height()*4);
    draw_to_display(screen);
#endif
}

void set_display_mode(const VideoMode &mode)
{
    if (mode.depth == 32
            && mode.red_mask_size == 8 && mode.green_mask_size == 8 && mode.blue_mask_size == 8
            && mode.red_field_pos == 16 && mode.green_field_pos == 8 && mode.blue_field_pos == 0)
    {
        if (mode.bytes_per_line != mode.width*mode.depth/CHAR_BIT)
        {
            warn("There is padding for this video mode, not enabling fast 32rgb draw\n");
        }
        else
        {
            draw_to_display = draw_to_display_32rgb_nopad;
        }
    }
}

void (*draw_to_display)(const Screen& screen) = draw_to_display_naive;

}
