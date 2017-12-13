/*
screen.cpp

Copyright (c) 10 Yann BOUCHER (yann)

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

#include "screen.hpp"

#include "utils/logging.hpp"

#include "graphics/drawing/display_draw.hpp"

namespace graphics
{

// TODO : alpha support
void Screen::resize(size_t width, size_t height, Color color)
{
    m_width = width;
    m_height = height;

    //posix_memalign(reinterpret_cast<void**>(&m_data), 32, width*height*sizeof(Color));
    m_data = reinterpret_cast<graphics::Color*>(kmalloc(width*height*sizeof(Color)));

    memset(data(), color.rgb(), width*height*4);
}

void Screen::blit(const Bitmap &bitmap, const PointU &pos)
{
    //    assert(bitmap.width() + pos.x <= width());
    //    assert(bitmap.height()+ pos.y <= height());

    size_t blit_width = std::min(bitmap.width(), width() - pos.x);
    size_t blit_height = std::min(bitmap.height(), height() - pos.y);

#if 0
    for (size_t i { 0 }; i < bitmap.width(); ++i)
    {
        for (size_t j { 0 }; j < bitmap.height(); ++j)
        {
            (*this)[{i + pos.x, j + pos.y}] = bitmap[{i, j}];
        }
    }
#else
    for (size_t j { 0 }; j < blit_height; ++j)
    {
        memcpyl(data() + (j+pos.y) * width() + (pos.x), bitmap.data() + (j*bitmap.width()),
                blit_width*sizeof(Color));
    }
#endif
}

#pragma GCC push_options
#pragma GCC optimize ("O3,tree-vectorize,omit-frame-pointer")
#pragma GCC target ("sse2")
void Screen::blit(const Bitmap &bitmap, const PointU &pos, const Color &white, const Color &transparent)
{
    for (size_t j { 0 }; j < bitmap.height(); ++j)
    {
        const size_t y_off = (j + pos.y)*width();

        for (size_t i { 0 }; i < bitmap.width(); ++i)
        {
            const auto& bmp = bitmap[{i, j}];
            m_data[y_off + i+pos.x] = (bmp.a == 0 ? transparent   :
                                                    bmp == color_white ? white :
                                                                         bmp);
        }
    }
}
#pragma GCC pop_options

}
