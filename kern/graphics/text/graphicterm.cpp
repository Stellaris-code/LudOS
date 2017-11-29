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

#include "utils/mathutils.hpp"

#include "time/timer.hpp"

#ifdef ARCH_i686
#include "i686/pc/devices/speaker.hpp"
#endif

namespace graphics
{

GraphicTerm::GraphicTerm(Screen &scr, const Font &font, TerminalData &data)
    : Terminal(scr.width() / font.glyph_width(), scr.height() / font.glyph_height(), data), m_scr(scr), m_font(font)
{
    m_cursor_bitmap.resize(2, font.glyph_height(), false, graphics::color_white);

    Timer::register_callback(600, [this]
    {
        m_show_cursor = !m_show_cursor;
        redraw_cursor();
        draw_impl();
    }, false);
}

void GraphicTerm::move_cursor(size_t x, size_t y)
{
    m_cursor_pos = {x*m_font.glyph_width(), y*m_font.glyph_height()};
}

void GraphicTerm::beep(size_t ms)
{
#ifdef ARCH_i686
    Speaker::beep(ms);
#endif
}

void GraphicTerm::putchar(size_t x, size_t y, TermEntry entry)
{
    const auto& bitmap = m_font.get(entry.c).bitmap;

    m_scr.blit(bitmap, {x*m_font.glyph_width(), y*m_font.glyph_height()},
               entry.pair.fg, entry.pair.bg);
}

void GraphicTerm::clear_line(size_t y, Color color)
{
    aligned_memsetl(m_scr.data() + y*m_scr.width()*m_font.glyph_height(),
            color.rgba(), m_scr.width()*m_font.glyph_height()*
            sizeof(Color));
}

void GraphicTerm::draw_impl()
{
    redraw_cursor();

    if (m_scr.allocated())
    {
        graphics::draw_to_display(m_scr);
    }
}

void GraphicTerm::redraw_cursor()
{
    m_scr.blit(m_cursor_bitmap, m_cursor_pos, (m_show_cursor ? term_data().color().fg : color_black),
               term_data().color().bg);
}

}
