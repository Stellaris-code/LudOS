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
#include <stdlib.h>
#include <string.h>

#include "utils/logging.hpp"

namespace graphics
{
Bitmap::Bitmap(size_t width, size_t height, Color color)
{
    resize(width, height, color);
}

//Bitmap::Bitmap(const Bitmap &other)
//{
//    m_width = other.width();
//    m_height = other.height();

//    if (m_data) kfree(m_data);
//    m_data = reinterpret_cast<Color*>(kmalloc(m_width * m_height * sizeof(Color)));
//    memcpyl(m_data, other.data(), m_width*m_height*sizeof(Color));
//}

Bitmap::~Bitmap()
{
    //kfree(m_data);
}

void Bitmap::resize(size_t width, size_t height, Color color)
{
    m_width = width;
    m_height = height;

    m_data.resize(width*height, color);
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
}
