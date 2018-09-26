/*
textterminal.cpp

Copyright (c) 17 Yann BOUCHER (yann)

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

#include "textterminal.hpp"

#include "i686/pc/devices/speaker.hpp"
#include "graphics/vga.hpp"

#include "termio.hpp"

TextTerminal::TextTerminal(uintptr_t fb, size_t iwidth, size_t iheight, TerminalData &data)
    : Terminal(iwidth, iheight, data), m_fb(fb)
{

}

void TextTerminal::move_cursor(size_t x, size_t y)
{
    termio_move_cursor(x, y, 80);
}

void TextTerminal::putchar(size_t x, size_t y, TermEntry entry)
{
    assert(x < 80);
    assert(y < 25);

    auto fb = reinterpret_cast<uint16_t*>(m_fb);
    fb[y * 80 + x] = graphics::vga::entry(entry.c, graphics::vga::entry_color(graphics::vga::color_to_vga(entry.pair.fg),
                                                                              graphics::vga::color_to_vga(entry.pair.bg)));
}

void TextTerminal::clear_line(size_t y, graphics::Color color, size_t size)
{
    assert(y < 25);

    auto fb = reinterpret_cast<uint16_t*>(m_fb);
    memsetw(fb + y*80, graphics::vga::entry(' ', graphics::vga::entry_color(graphics::vga::color_to_vga(color),
                                graphics::vga::color_to_vga(color))), size * sizeof(uint16_t));
}

void TextTerminal::draw_impl()
{

}
