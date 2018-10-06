/*
bitmap.cpp

Copyright (c) 19 Yann BOUCHER (yann)

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

#include "bitmap.hpp"

#include <assert.h>

#include "utils/logging.hpp"

#include "stb/stb_image_resize.h"

namespace graphics
{
Bitmap::Bitmap(size_t width, size_t height, Color color)
{
    resize(width, height, false, color);
}

void Bitmap::resize(size_t width, size_t height, bool keep_ratio, Color color)
{
    if (width == m_width && height == m_height) return;

    if (!keep_ratio)
    {
        m_data.resize(width*height, color);
    }
    else
    {
        std::vector<graphics::Color> copy = m_data;

        m_data.resize(width*height);

        asm volatile ("cli\n");
        stbir_resize_uint8(reinterpret_cast<const uint8_t*>(copy.data()), m_width, m_height, 0,
                           reinterpret_cast<uint8_t*>(m_data.data()), width, height, 0, 4);
        asm volatile ("sti\n");
    }

    m_width = width;
    m_height = height;
}

Bitmap Bitmap::copy_rect(const PointU &pos, const PointU &size) const
{
    assert(pos.x + size.x <= width() && pos.y + size.y <= height());

    Bitmap bmp(size.x, size.y);

    for (size_t i { 0 }; i < size.y; ++i)
    {
        memcpyl(bmp.data() + i*bmp.width(),
                data() + (pos.y+i)*width() + pos.x, bmp.width()*sizeof(Color));
    }

    return bmp;
}



void Bitmap::color_multiply(const Color &color)
{
    for (size_t i { 0 }; i < width()*height(); ++i)
    {
        auto& pix = m_data[i];
        pix.r = pix.r * color.r / 255;
        pix.g = pix.g * color.g / 255;
        pix.b = pix.b * color.b / 255;
    }
}

void Bitmap::color_blend(const Color &white, const Color &transparent)
{
    for (size_t i { 0 }; i < width()*height(); ++i)
    {
        auto& pix = m_data[i];
        if (pix == color_white)
        {
            pix = white;
        }
        else if (pix.a == 0)
        {
            pix = transparent;
        }
    }
}

void blit(Color* buf, size_t buf_width, size_t buf_height, const Bitmap &bitmap, const PointU &pos)
{
    //    assert(bitmap.width() + pos.x <= width());
    //    assert(bitmap.height()+ pos.y <= height());

    size_t blit_width = std::min(bitmap.width(), buf_width - pos.x);
    size_t blit_height = std::min(bitmap.height(), buf_height - pos.y);

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
        memcpyl(buf + (j+pos.y) * buf_width + (pos.x), bitmap.data() + (j*bitmap.width()),
                blit_width*sizeof(Color));
    }
#endif
}

#pragma GCC push_options
#pragma GCC optimize ("O3,tree-vectorize,omit-frame-pointer")
#pragma GCC target ("sse2")
void blit(Color* buf, size_t buf_width, size_t buf_height, const Bitmap &bitmap, const PointU &pos, const Color &white)
{
    (void)buf_height;
    for (size_t j { 0 }; j < bitmap.height(); ++j)
    {
        const size_t y_off = (j + pos.y)*buf_width;

        for (size_t i { 0 }; i < bitmap.width(); ++i)
        {
            const auto& bmp = bitmap[{i, j}];
            if (bmp.a != 0) buf[y_off + i+pos.x] = (bmp == color_white ? white : bmp);
        }
    }
}
#pragma GCC pop_options

#pragma GCC push_options
#pragma GCC optimize ("O3,tree-vectorize,omit-frame-pointer")
#pragma GCC target ("sse2")
void blit(Color* buf, size_t buf_width, size_t buf_height, const Bitmap &bitmap, const PointU &pos, const Color &white, const Color &transparent)
{
    (void)buf_height;
    for (size_t j { 0 }; j < bitmap.height(); ++j)
    {
        const size_t y_off = (j + pos.y)*buf_width;

        for (size_t i { 0 }; i < bitmap.width(); ++i)
        {
            const auto& bmp = bitmap[{i, j}];
            buf[y_off + i+pos.x] = (bmp.a == 0 ? transparent :
                                                    bmp == color_white ? white : bmp);
        }
    }
}
#pragma GCC pop_options
}
