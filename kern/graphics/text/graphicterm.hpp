/*
graphicterm.hpp

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
#ifndef GRAPHICTERM_HPP
#define GRAPHICTERM_HPP

#include "terminal/terminal.hpp"

#include "graphics/fonts/font.hpp"
#include "graphics/drawing/screen.hpp"
#include "graphics/drawing/bitmap.hpp"

#include "time/timer.hpp"

#include "utils/messagebus.hpp"

namespace graphics
{

class GraphicTerm : public Terminal
{
public:

public:
    GraphicTerm(Screen& scr, TerminalData &data, const Font& font = default_font());
    ~GraphicTerm();

public:
    static Font& default_font();

private:
    virtual void move_cursor(size_t x, size_t y) override;
    virtual void putchar(size_t x, size_t y, TermEntry entry) override;
    virtual void clear_line(size_t y, Color color) override;
    virtual void draw_impl() override;
    virtual void disable_impl() override;

private:
    void redraw_cursor();
    void update_background();
    void set_wallpaper(const Bitmap& bitmap);

private:
    Screen& m_scr;
    const Font& m_font;

    volatile bool m_show_cursor { false };
    PointU m_cursor_pos { 0, 0 };
    Bitmap m_cursor_bitmap;
    Bitmap m_background;
    kpp::string m_background_path;
    Timer::CallbackHandle m_callback;
    MessageBus::RAIIHandle m_msg_handle;
};

}

#endif // GRAPHICTERM_HPP
