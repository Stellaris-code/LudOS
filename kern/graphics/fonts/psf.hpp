/*
psf.hpp

Copyright (c) 07 Yann BOUCHER (yann)

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
#ifndef PSF_HPP
#define PSF_HPP

#include "font.hpp"

#include <unordered_map.hpp>

#include "utils/membuffer.hpp"

namespace graphics::psf
{

class PSFFont : public Font
{
public:
    PSFFont();

    [[nodiscard]] virtual bool load(const kpp::string& path) override;

    virtual size_t glyph_width() const override { return m_hdr->width; }
    virtual size_t glyph_height() const override{ return m_hdr->height; }

private:
    virtual Glyph read_glyph(char32_t c) const override;

private:
    bool load_psf();
    void build_unicode_table();
    Glyph read_glyph_data(const uint8_t* ptr) const;

private:
    struct header
    {
        uint8_t magic[4];
        uint32_t version;
        uint32_t headersize;    /* offset of bitmaps in file */
        uint32_t flags;
        uint32_t length;	    /* number of glyphs */
        uint32_t charsize;	    /* number of bytes for each character */
        uint32_t height, width; /* max dimensions of glyphs */
        /* charsize = height * ((width + 7) / 8) */
    };

private:
    std::unordered_map<uint32_t, size_t> m_unicode_jump_table;
    MemBuffer m_data;
    const header* m_hdr { nullptr };
};

}

#endif // PSF_HPP
