/*
font.hpp

Copyright (c) 08 Yann BOUCHER (yann)

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
#ifndef FONT_HPP
#define FONT_HPP

#include "graphics/drawing/bitmap.hpp"

#include <string.hpp>
#include <vector.hpp>
#include <memory.hpp>

#include "utils/logging.hpp"

namespace graphics
{

struct Glyph
{
    Glyph(Bitmap&& bmp)
        : bitmap(std::move(bmp))
    {
    }

    Glyph(const Glyph& g) = delete;
    Glyph(Glyph&& g) = default;

    Bitmap bitmap;
};

class Font
{
public:
    Font()
    {
        m_font_cache.resize(0x10000);
    }

    [[nodiscard]] virtual bool load(const std::string& path) = 0;

    const Glyph& get(char32_t c) const;

    virtual size_t glyph_width() const = 0;
    virtual size_t glyph_height() const = 0;

private:
    virtual Glyph read_glyph(char32_t c) const = 0;

private:
    mutable std::vector<std::unique_ptr<Glyph>> m_font_cache;
};

}

#endif // FONT_HPP
