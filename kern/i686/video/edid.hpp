/*
edid.hpp

Copyright (c) 04 Yann BOUCHER (yann)

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
#ifndef EDID_HPP
#define EDID_HPP

#include <stdint.h>

#include <utility.hpp>
#include <optional.hpp>

#include "graphics/video.hpp"

struct EDIDInfo
{
    uint8_t header[8];
    uint8_t manufacturer_id[2];
    uint16_t product_code;
    uint32_t serial_number;
    uint8_t week;
    uint8_t year;
    uint8_t edid_version;
    uint8_t edid_rev;
    uint8_t input_type;
    uint8_t max_honz_size;
    uint8_t max_vert_size;
    uint8_t gamma;
    uint8_t features;
    uint8_t chroma_info[10];
    uint8_t established_timings_1;
    uint8_t established_timings_2;
    uint16_t standard_timings[8];
};

namespace EDID
{

std::optional<EDIDInfo> get();

graphics::MonitorInfo to_monitor_info(const EDIDInfo& info);

}

#endif // EDID_HPP
