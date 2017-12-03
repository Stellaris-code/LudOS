/*
terminal.cpp

Copyright (c) 16 Yann BOUCHER (yann)

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

#include "terminal.hpp"

#include <ctype.h>

#include "nullterm.hpp"

#include "utils/logging.hpp"
#include "utils/messagebus.hpp"
#include "utils/stlutils.hpp"
#include "unicode/utf32decoder.hpp"

std::unique_ptr<Terminal> current_term;
std::unique_ptr<TerminalData> current_termdata;

// TODO : editing line wrapping
// TODO : bouger le curseur

Terminal::Terminal(size_t iwidth, size_t iheight, TerminalData &data)
    : m_width(iwidth), m_height(iheight), m_data(data)
{
}

void Terminal::put_char(char32_t c)
{
    if (m_escape_code)
    {
        if (c >= 0x40 && c <= 0x7E)
        {
            m_escape_code = false;
            process_escape_code();
            m_escape_sequence.clear();
        }
        else
        {
            m_escape_sequence.push_back(c);
        }
    }

    if (c == '\e')
    {
        m_escape_code = true;
    }
    else if (c == '\n')
    {
        new_line();
    }
    else if (c == '\r')
    {
        m_cursor_x = 0;
    }
    else if (c == '\b')
    {
        if (m_cursor_x > m_input_off)
        {
            --m_cursor_x;

            check_pos();
            set_entry_at({' ', m_data.color()}, m_cursor_x, m_cursor_y);
            m_cur_line.pop_back();
        }
    }
    else if (c == '\t')
    {
        for (size_t i { 0 }; i < tab_size; ++i)
        {
            put_char(' ');
        }
    }
    else if (c == '\a')
    {
        beep(200);
    }
    else
    {
        check_pos();
        set_entry_at({c, m_data.color()}, m_cursor_x, m_cursor_y);
        m_cur_line.push_back({c, m_data.color()});
        ++m_cursor_x;
    }

    check_pos();
}

void Terminal::add_input(char32_t c)
{
    switch_to_input();
    put_char(c);
}

void Terminal::clear_input()
{
    while (m_cur_line.size() > m_input_off)
    {
        m_cur_line.pop_back();
        --m_cursor_x;
    }

    force_redraw();
}

void Terminal::set_input(const std::string &str)
{
    while (m_cur_line.size() > m_input_off)
    {
        m_cur_line.pop_back();
        --m_cursor_x;
    }

    switch_to_input();

    write_string(str.c_str());

    force_redraw();
}

void Terminal::switch_to_input()
{
    if (!m_line_is_input)
    {
        m_line_is_input = true;
        m_input_off = m_cursor_x;
    }
}

void Terminal::write(const char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        m_decoder.feed(data[i]);
        if (m_decoder.ready())
        {
            char32_t c = m_decoder.spit();
            put_char(c);
        }
    }
}


void Terminal::write_string(const char *data)
{
    write(data, strlen(data));
}

void Terminal::clear()
{
    clear(m_data.color());
}

void Terminal::clear(ColorPair color)
{
    m_cursor_x = m_cursor_y = 0;
    m_current_history_page = 0;
    m_cur_line.clear();
    m_decoder.reset();

    for (size_t i { 0 }; i < true_height(); ++i)
    {
        clear_line(i, color.bg);
    }

    draw();
}

void Terminal::scroll_up()
{
    scroll_history(+1);
}

void Terminal::scroll_bottom()
{
    show_history(m_data.lines() - height()+1);
    m_cursor_y = m_data.lines() - height()+1;
    check_pos();
}


void Terminal::show_history(int page)
{
    if (m_scrolling)
    {
        if (page < 0)
        {
            page = 0;
        }

        if (static_cast<size_t>(page) > m_data.lines() - height()+1)
        {
            page = m_data.lines() - height()+1; // avoir un plafond, une limite
        }

        m_current_history_page = page;

        auto screen = m_data.get_screen(width(), height(), page);

        for (size_t i { 0 }; i < screen.size(); ++i)
        {
            clear_line(i + m_data.title_height, m_data.color().bg);
            for (size_t j { 0 }; j < screen[i].size(); ++j)
            {
                set_entry_at(screen[i][j], j, i);
            }
        }

        clear_line(true_height()-1, m_data.color().bg);

        force_redraw_input();
    }
}

size_t Terminal::current_history() const
{
    return m_current_history_page;
}

void Terminal::scroll_history(int scroll)
{
    show_history(current_history()+scroll);
    draw();
}

void Terminal::set_title(std::u32string str, ColorPair color)
{
    m_data.title_str = str;

    if (str.size() >= width())
    {
        str.resize(width());
    }

    m_data.title_color = color;

    size_t offset = width()/2 - str.size()/2;

    for (size_t j { 0 }; j < m_data.title_height; ++j)
    {
        clear_line(j, color.bg);
    }

    for (size_t i { 0 }; i < str.size(); ++i)
    {
        set_entry_at({str[i], color}, offset+i, 0, true);
    }

    draw();
}

void Terminal::set_title(std::u32string str)
{
    set_title(str, m_data.color());
}

std::string Terminal::input() const
{
    std::string str;
    for (size_t i { m_input_off }; i < m_cur_line.size(); ++i)
    {
        str += decode_utf32(m_cur_line[i].c);
    }
    return str;
}

void Terminal::set_entry_at(TermEntry entry, size_t x, size_t y, bool absolute)
{
    if (m_enabled)
    {
        putchar(x, y + (absolute ? 0 : m_data.title_height), entry);
        m_dirty = true;
    }
}

void Terminal::new_line()
{
    if (!m_line_is_input)
    {
        add_line_to_history();
    }

    check_pos();

    m_cursor_x = 0;
    ++m_cursor_y;

    check_pos();

    scroll_up();

    if (m_line_is_input)
    {
        m_line_is_input = false;
        //put_char('\n');
        TermInputEvent ev;
        ev.line = input();

        MessageBus::send(ev);
    }

    for (size_t i { 0 }; m_cursor_x + i < width(); ++i)
    {
        set_entry_at({' ', m_data.color()}, m_cursor_x + i, m_cursor_y);
    }
    m_cur_line.clear();

    force_redraw();
}

void Terminal::add_line_to_history()
{
    m_data.add_line(m_cur_line);
}

void Terminal::check_pos()
{
    if (m_cursor_y >= height())
    {
        m_cursor_y = height()-1;
    }

    update_cursor();
}

void Terminal::update_cursor()
{
    move_cursor(m_cursor_x, m_cursor_y + m_data.title_height);
}

void Terminal::reset()
{
    clear({0xaaaaaa, 0xaaaaaa});

    m_cur_line.clear();
}

void Terminal::process_escape_code()
{
    auto param_list = tokenize(m_escape_sequence, ";");
}

void Terminal::resize(size_t iwidth, size_t iheight)
{
    assert(iwidth && iheight);

    reset();

    m_width = iwidth;
    m_height = iheight;

    set_title(m_data.title_str, m_data.title_color);

    if (m_cursor_x >= width())
    {
        m_cursor_x = width()-1;
    }
    check_pos();

    force_redraw();
}

size_t Terminal::width() const
{
    return m_width;
}

size_t Terminal::height() const
{
    return m_height - m_data.title_height;
}

size_t Terminal::true_height() const
{
    return m_height;
}

void Terminal::force_redraw()
{
    update_cursor();

    show_history(m_current_history_page);

    force_redraw_input();
}

void Terminal::force_redraw_input()
{
    clear_line(m_cursor_y + m_data.title_height, m_data.color().bg);

    for (size_t i { 0 }; i < m_cur_line.size(); ++i)
    {
        set_entry_at(m_cur_line[i], i, m_cursor_y);
    }

    draw();
}

void Terminal::draw()
{
    if (m_enabled && m_dirty)
    {
        m_dirty = false;
        draw_impl();
    }
}

Terminal& term()
{
    if (current_term == nullptr)
    {
        static TerminalData dummy(1);
        create_term<NullTerminal>(dummy);
    }

    assert(current_term);
    return *current_term;
}

TerminalData &term_data()
{
    if (!current_termdata)
    {
        current_termdata = std::make_unique<TerminalData>(1000);
    }

    return *current_termdata;
}

void reset_term()
{
    create_term<NullTerminal>(term_data());
}
