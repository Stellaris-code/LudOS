/*
psf.cpp

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

#include "psf.hpp"

#include "fs/fsutils.hpp"
#include "fs/vfs.hpp"

#include "graphics/drawing/bitmap.hpp"
#include "utils/bitops.hpp"
#include "utils/memutils.hpp"

#define PSF2_MAGIC0     0x72
#define PSF2_MAGIC1     0xb5
#define PSF2_MAGIC2	0x4a
#define PSF2_MAGIC3	0x86

/* bits used in flags */
#define PSF2_HAS_UNICODE_TABLE 0x01

/* max version recognized so far */
#define PSF2_MAXVERSION	0

/* UTF8 separators */
#define PSF2_SEPARATOR  0xFF
#define PSF2_STARTSEQ   0xFE


#define PSF2_MAGIC_OK(x)	((x)[0]==PSF2_MAGIC0 && (x)[1]==PSF2_MAGIC1 \
    && (x)[2]==PSF2_MAGIC2 && (x)[3]==PSF2_MAGIC3)

namespace graphics::psf
{

PSFFont::PSFFont()
    : Font()
{
}

bool PSFFont::load(const kpp::string &path)
{
    auto file = vfs::find(path);
    if (!file)
    {
        log_serial("Path : %s\n", path.c_str());
        return false;
    }

    m_data = file.value()->read();
    if (m_data.empty()) return false;

    return load_psf();
}

bool PSFFont::load_psf()
{
    m_hdr = reinterpret_cast<const header*>(m_data.data());
    if (!PSF2_MAGIC_OK(m_hdr->magic) || m_hdr->version > PSF2_MAXVERSION) return false;

    if (m_hdr->flags & PSF2_HAS_UNICODE_TABLE)
    {
        build_unicode_table();
    }

    return true;
}

void PSFFont::build_unicode_table()
{
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(m_hdr)
            + m_hdr->headersize + m_hdr->length * m_hdr->charsize;

    char32_t glyph = 0;
    while(ptr < m_data.data() + m_data.size())
    {
        char32_t ch = ptr[0]&0xFF;
        if(ch == 0xFF)
        {
            glyph++;
            ptr++;
            continue;
        }
        else if(ch & 128)
        {
            /* UTF-8 to unicode */
            if((ch & 32) == 0 )
            {
                ch = ((ptr[0] & 0x1F)<<6)+(ptr[1] & 0x3F);
                ptr++;
            }
            else
            {
                if((ch & 16) == 0 )
                {
                    ch = ((((ptr[0] & 0xF)<<6)+(ptr[1] & 0x3F))<<6)+(ptr[2] & 0x3F);
                    ptr+=2;
                }
                else
                {
                    if((ch & 8) == 0 )
                    {
                        ch = ((((((ptr[0] & 0x7)<<6)+(ptr[1] & 0x3F))<<6)+(ptr[2] & 0x3F))<<6)+(ptr[3] & 0x3F);
                        ptr+=3;
                    }
                    else
                    {
                        ch = 0;
                    }
                }
            }
        }
        /* save translation */
        m_unicode_jump_table[ch] = glyph;
        ptr++;
    }
}

Glyph PSFFont::read_glyph(char32_t c) const
{
    size_t glyph = c;
    if (m_hdr->flags & PSF2_HAS_UNICODE_TABLE)
    {
        auto it = m_unicode_jump_table.find(c);
        if (it != m_unicode_jump_table.end())
        {
            glyph = it->second;
        }
        else
        {
            return read_glyph_data(reinterpret_cast<const uint8_t*>(m_hdr) + m_hdr->headersize);
        }
    }
    if (glyph >= m_hdr->length)
    {
        return read_glyph_data(reinterpret_cast<const uint8_t*>(m_hdr) + m_hdr->headersize);
    }

    return read_glyph_data(reinterpret_cast<const uint8_t*>(m_hdr) + m_hdr->headersize + m_hdr->charsize*glyph);
}

Glyph PSFFont::read_glyph_data(const uint8_t *ptr) const
{
    Bitmap bitmap(glyph_width(), glyph_height(), color_transparent);

    size_t bytes_per_line = m_hdr->charsize/m_hdr->height;

    for (size_t j { 0 }; j < glyph_height(); ++j)
    {
        for (size_t i = 0; i < glyph_width(); ++i)
        {
            size_t byte_index = i / CHAR_BIT;
            size_t bit_offset = i % CHAR_BIT;

            if (bit_check(ptr[j*bytes_per_line + byte_index], CHAR_BIT-1-bit_offset))
            {
                bitmap[{i, j}] = color_white;
            }
        }
    }

    return Glyph{std::move(bitmap)};
}
}
