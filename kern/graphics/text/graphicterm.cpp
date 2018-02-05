/*
graphicterm.cpp

Copyright (c) 18 Yann BOUCHER (yann)

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

#include "graphicterm.hpp"

#include "graphics/drawing/display_draw.hpp"
#include "graphics/drawing/bitmap.hpp"
#include "graphics/drawing/image_loader.hpp"

#include "utils/mathutils.hpp"

#include "time/timer.hpp"
#include "graphics/fonts/psf.hpp"
#include "utils/env.hpp"

namespace graphics
{

GraphicTerm::GraphicTerm(Screen &scr, TerminalData &data, const Font &font)
    : Terminal(scr.width() / font.glyph_width(), scr.height() / font.glyph_height(), data), m_scr(scr), m_font(font)
{
    m_cursor_bitmap.resize(1, font.glyph_height(), false, graphics::color_white);

    m_callback = Timer::register_callback(600, [this]
    {
        if (enabled())
        {
            m_show_cursor = !m_show_cursor;
            redraw_cursor();
            draw_impl();
        }
    }, false);

    Bitmap black(m_scr.width(), m_scr.height(), color_black);
    set_wallpaper(black);

    update_background();

    m_msg_handle = MessageBus::register_handler<EnvVarChange>([this](const EnvVarChange& msg)
    {
        if (msg.key == "TERM_BCKG") update_background();
    });
}

GraphicTerm::~GraphicTerm()
{
    Timer::remove_callback(m_callback);
}

Font &GraphicTerm::default_font()
{
    static psf::PSFFont font;
    static bool initialized = false;
    if (!initialized)
    {
        assert(font.load("/initrd/system.8x16.psf"));
        initialized = true;
    }

    return font;
}

void GraphicTerm::set_wallpaper(const Bitmap &bitmap)
{
    m_background = bitmap;
    m_background.resize(m_scr.width(), m_scr.height());
}

void GraphicTerm::move_cursor(size_t x, size_t y)
{
    m_cursor_pos = {x*m_font.glyph_width(), y*m_font.glyph_height()};
}

void GraphicTerm::putchar(size_t x, size_t y, TermEntry entry)
{
    const auto& bitmap = m_font.get(entry.c).bitmap;

    if (entry.pair.bg.rgb() != color_black.rgb())
    {
        m_scr.blit(bitmap, {x*m_font.glyph_width(), y*m_font.glyph_height()},
                   entry.pair.fg, entry.pair.bg);
    }
    else
    {
        m_scr.blit(bitmap, {x*m_font.glyph_width(), y*m_font.glyph_height()},
                   entry.pair.fg);
    }
}

void GraphicTerm::clear_line(size_t y, Color color)
{
    if (color == term_data().color().bg)
    {
        for (size_t i { 0 }; i < m_font.glyph_height(); ++i)
        {
            aligned_memcpy(m_scr.data() + (y*m_font.glyph_height() + i)*m_scr.width(),
                           m_background.data() + (y*m_font.glyph_height() + i)*m_scr.width(),
                           m_scr.width()*sizeof(Color));
        }
    }
    else
    {
        aligned_memsetl(m_scr.data() + y*m_scr.width()*m_font.glyph_height(),
                        color.rgba(), m_scr.width()*m_font.glyph_height()*
                        sizeof(Color));
    }
}

void GraphicTerm::draw_impl()
{
    redraw_cursor();

    graphics::draw_to_display(m_scr);
}

void GraphicTerm::disable_impl()
{

}

void GraphicTerm::redraw_cursor()
{
    if (m_show_cursor)
    {
        m_scr.blit(m_cursor_bitmap, m_cursor_pos, term_data().color().fg);
    }
    else
    {
        auto bckg_bitmap = m_background.copy_rect(m_cursor_pos, {m_cursor_bitmap.width(), m_cursor_bitmap.height()});
        m_scr.blit(bckg_bitmap, m_cursor_pos);
    }
}

void GraphicTerm::update_background()
{
    auto bckg = kgetenv("TERM_BCKG");
    if (bckg && *bckg != m_background_path)
    {
        if (bckg->empty())
        {
            Bitmap black(m_scr.width(), m_scr.height(), color_black);
            set_wallpaper(black);

            return;
        }

        auto img = graphics::load_image(*bckg);
        if (!img)
        {
            warn("Invalid terminal background path : '%s'\n", bckg->c_str());
            return;
        }

        set_wallpaper(*img);
        m_background_path = *bckg;
    }
}

}
