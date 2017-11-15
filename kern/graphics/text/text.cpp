/*
text.cpp

Copyright (c) 11 Yann BOUCHER (yann)

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

#include "text.hpp"

#include "terminal/terminal.hpp"
#include "graphics/drawing/screen.hpp"
#include "graphics/drawing/bitmap.hpp"
#include "graphics/drawing/display_draw.hpp"
#include "graphics/video.hpp"

namespace graphics
{

void setup_terminal(const Font &font, Screen& scr)
{
    assert(font.glyph_height() && font.glyph_width());

    size_t text_cols  = current_video_mode().width / font.glyph_width();
    size_t text_lines = current_video_mode().height / font.glyph_height();

    // TODO : do
    //setup_term(text_cols, text_lines, term().history_lines());

    term().putchar_callback = [text_cols, text_lines, &font, &scr](size_t x, size_t y, char32_t c, TermEntry color)
    {
        if (x >= text_cols || y >= text_lines) return;

        auto bitmap = font.get(c).bitmap;
        bitmap.color_set_white(color.fg);
        bitmap.color_set_transparent(color.bg);

        scr.blit(bitmap, {x*font.glyph_width(), y*font.glyph_height()});
    };

    term().redraw_callback = [&scr]
    {
        graphics::draw_to_display(scr);
    };
}

}
