/*
video.hpp

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
#ifndef VIDEO_HPP
#define VIDEO_HPP

#include <stdint.h>

#include <vector.hpp>
#include <optional.hpp>

#include "color.hpp"

namespace graphics
{

struct VideoMode
{
    uintptr_t phys_fb_addr { 0 };
    uintptr_t virt_fb_addr { 0 };
    uint32_t width { 0 };
    uint32_t height { 0 };
    uint32_t depth { 0 };
    uint32_t bytes_per_line { 0 };
    uint8_t  red_mask_size { 0 };
    uint8_t  red_field_pos { 0 };
    uint8_t  green_mask_size { 0 };
    uint8_t  green_field_pos { 0 };
    uint8_t  blue_mask_size { 0 };
    uint8_t  blue_field_pos { 0 };
    enum
    {
        Text,
        Graphics
    } type;
};

struct MonitorInfo
{
    size_t width;
    size_t height;
};

constexpr size_t max_res_pixels { 1920 * 1080 * 4 };
class Screen;

std::vector<VideoMode> list_video_modes();
[[nodiscard]] kpp::optional<VideoMode> change_mode(size_t width, size_t height, size_t depth);
Screen* screen();
VideoMode current_video_mode();
kpp::optional<MonitorInfo> monitor_info();

}

#endif // VIDEO_HPP
