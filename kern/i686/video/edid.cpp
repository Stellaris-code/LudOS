/*
edid.cpp

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

#include "edid.hpp"

#include "x86emu_modesetting.hpp"
#include "vbe.hpp"

namespace EDID
{

kpp::optional<EDIDInfo> get()
{
    memset(emu_mem + 0x20000, 0, sizeof(EDIDInfo));
    EDIDInfo* block = reinterpret_cast<EDIDInfo*>(emu_mem + 0x20000);

    auto state = emuInt10h(0x4F15, 0x01, 0, 0, 0x2000);
    const auto& emu = state.cpu_state.emu;

    if (emu->x86.R_AL == VBE_RETURN_STATUS_SUPPORTED)
    {
        if (emu->x86.R_AH == VBE_RETURN_STATUS_SUCCESSFULL)
        {
            return *block;
        }
        else
        {
            log(Debug, "EDID Info call failed, code 0x%x\n", emu->x86.R_AH);
            return {};
        }
    }
    else
    {
        log(Debug, "EDID Info interrupt not supported\n");
        return {};
    }
}

graphics::MonitorInfo to_monitor_info(const EDIDInfo &info)
{
    std::pair<size_t, size_t> ratio;
    switch(((info.standard_timings[0]>>8)&0x11000000)>>6)
    {
        case 0:
            ratio = {16, 10};
            break;
        case 1:
            ratio = {4, 3};
            break;
        case 2:
            ratio = {5, 4};
            break;
        case 3:
        default:
            ratio = {16, 9};
            break;
    }

    log_serial("data : 0x%x\n", info.standard_timings[0]);

    size_t mon_width = size_t((info.standard_timings[0]&0xFF)+31)*8;
    size_t mon_height = mon_width*ratio.second/ratio.first;

    return graphics::MonitorInfo{
        .width = mon_width,
        .height = mon_height
    };
}

}
