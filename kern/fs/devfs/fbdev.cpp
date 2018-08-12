/*
fbdev.cpp

Copyright (c) 12 Yann BOUCHER (yann)

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

#include "fbdev.hpp"

#include "graphics/video.hpp"
#include "tasking/process.hpp"

#include "halt.hpp"
#include "utils/logging.hpp"

#pragma GCC push_options
#pragma GCC target ("no-sse")

FBDevMode to_fbdevmode(const graphics::VideoMode& mode)
{
    FBDevMode fbmode;
    fbmode.height = mode.height;
    fbmode.width = mode.width;
    fbmode.depth = mode.depth;
    fbmode.bytes_per_line = mode.bytes_per_line;
    fbmode.red_mask_size = mode.red_mask_size;
    fbmode.green_mask_size = mode.green_mask_size;
    fbmode.blue_mask_size = mode.blue_mask_size;
    fbmode.red_field_pos = mode.red_field_pos;
    fbmode.green_field_pos = mode.green_field_pos;
    fbmode.blue_field_pos = mode.blue_field_pos;

    return fbmode;
}

int fbdev_node::get_video_modes(FBDevMode *buffer, size_t buffer_count) const
{
    auto modes = graphics::list_video_modes();
    const size_t amount = std::min(modes.size(), buffer_count);

    size_t buffer_counter { 0 };

    for (size_t i { 0 }; i < modes.size(); ++i)
    {
        if (buffer_counter >= amount)
            break;

        auto mode = modes[i];
        if (mode.type == graphics::VideoMode::Text)
            continue;

        buffer[buffer_counter] = to_fbdevmode(mode);

        ++buffer_counter;
    }
    return buffer_counter;
}

int fbdev_node::get_current_mode(FBDevMode *mode) const
{
    *mode = to_fbdevmode(graphics::current_video_mode());
    return 0;
}

int fbdev_node::switch_mode(int width, int height, int depth) const
{
    bool result = !!graphics::change_mode(width, height, depth);
    return result ? 0 : -1;
}

uint8_t *fbdev_node::get_framebuffer() const
{
    auto current = graphics::current_video_mode();
    uintptr_t address = (uintptr_t)Process::current().map_range(current.phys_fb_addr, current.bytes_per_line*current.height);
    return (uint8_t*)address;
}

#pragma pop_options
