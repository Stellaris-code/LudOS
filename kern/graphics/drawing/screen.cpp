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
void Screen::blit(const Bitmap &bitmap, const PointU &pos, bool redraw)
{
    assert(bitmap.width() + pos.x < width());
    assert(bitmap.height() + pos.y < height());

#if 0
    for (size_t i { 0 }; i < bitmap.width(); ++i)
    {
        for (size_t j { 0 }; j < bitmap.height(); ++j)
        {
            (*this)[{i + pos.x, j + pos.y}] = bitmap[{i, j}];
            (*this)[{i + pos.x, j + pos.y}].a = 0;
        }
    }
#else
    for (size_t j { 0 }; j < bitmap.height(); ++j)
    {
        memcpy(data() + (j+pos.y) * width() + (pos.x), bitmap.data() + (j*bitmap.width()),
               bitmap.width()*sizeof(Color));
    }
#endif

    if (redraw)
    {
        graphics::draw_to_display(*this);
    }
}

}
