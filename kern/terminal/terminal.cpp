/*
Terminal.cpp

Copyright (c) 23 Yann BOUCHER (yann)

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

#include <string.h>
#include <ctype.h>
#include <string.hpp>

#include "utils/logging.hpp"

#include "io.hpp"
#include "halt.hpp"

std::unique_ptr<Terminal> current_term { nullptr };

Terminal::Terminal(size_t iwidth, size_t iheight, size_t imax_history)
    : _width(iwidth), _height(iheight), max_history(imax_history),
      history(_width, height()*max_history)
{
    putchar_callback = [](size_t, size_t, char32_t, TermEntry){};
    redraw_callback = []{};
    push_color(TermEntry{0xaaaaaa, 0});
    cur_line.resize(width());
}


void Terminal::put_entry_at(char32_t c, TermEntry color, size_t x, size_t y)
{
    check_pos();
    set_entry_at(c, color, x, y);
    if (y == terminal_row)
    {
        cur_line[x] = {c, color};
    }
}


void Terminal::put_char(char32_t c)
{
    if (c == '\n')
    {
        new_line();
    }
    else if (c == '\r')
    {
        terminal_column = 0;
    }
    else if (c == '\b')
    {
        if (terminal_column == 0)
        {
            if (terminal_row > 0)
            {
                //--terminal_row;
            }
        }
        else
        {
            --terminal_column;
        }

        put_entry_at(' ', color(), terminal_column, terminal_row);

        check_pos();
    }
    else if (c == '\t')
    {
        terminal_column += 4;
    }
    else if (c == '\a')
    {
        if (beep_callback)
        {
            beep_callback(200);
        }
    }
    else if (isprint(c))
    {
        put_entry_at(c, color(), terminal_column, terminal_row);
        ++terminal_column;
    }

    check_pos();
}


void Terminal::write(const char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        decoder.feed(data[i]);
        if (decoder.ready())
        {
            put_char(decoder.spit());
        }
    }
}


void Terminal::write_string(const char *data)
{
    write(data, strlen(data));
}


void Terminal::clear()
{
    clear(color());
}

void Terminal::clear(TermEntry color)
{
    history.clear();

    for (size_t i { 0 }; i < width(); ++i)
    {
        for (size_t j { 0 }; j < height()+title_height; ++j)
        {
            set_entry_at(' ', color, i, j, true);
        }
    }

    for (auto& el : cur_line)
    {
        el = {' ', color};
    }

    redraw();
}


void Terminal::scroll_up()
{
    scroll_history(-1);
}


void Terminal::push_color(TermEntry color)
{
    color_stack.push(color);
}


void Terminal::pop_color()
{
    color_stack.pop();
}


void Terminal::show_history(int page)
{
    if (page < 0)
    {
        if (beep_callback && enabled)
        {
            //beep_callback(200);
        }
        page = 0;
    }

    if (static_cast<size_t>(page) > history.size() - height())
    {
        if (beep_callback && enabled)
        {
            //            beep_callback(200);
        }
        page = history.size() - height(); // avoir un plafond, une limite

    }

    current_history_page = page;

    for (size_t i { 0 }; i < height()-1; ++i)
    {
        int index = history.size() - (height()-i) - page;
        if (index >= 0)
        {
            for (size_t j { 0 }; j < width(); ++j)
            {
                set_entry_at(history.get_char(j, index).c, history.get_char(j, index).color, j, i);
            }
        }
    }

    redraw();
}

TermEntry Terminal::color() const
{
    return color_stack.top();
}

void Terminal::set_title(std::string str, TermEntry color)
{
    if (str.size() >= width())
    {
        str.resize(width());
    }

    text_color = color;

    title_text = str;

    size_t offset = width()/2 - str.size()/2;

    for (size_t j { 0 }; j < title_height; ++j)
    {
        for (size_t i { 0 }; i < width(); ++i)
        {
            set_entry_at(' ', color, i, j, true);
        }
    }

    for (size_t i { 0 }; i < str.size(); ++i)
    {
        set_entry_at(str[i], color, offset+i, 0, true);
    }

    redraw();
}

void Terminal::set_title(std::string str)
{
    set_title(str, color());
}

void Terminal::redraw()
{
    redraw_callback();
}

void Terminal::set_entry_at(char32_t c, TermEntry color, size_t x, size_t y, bool absolute)
{
    if (enabled)
    {
        if (!absolute)
        {
            putchar_callback(x, y + title_height, c, color);
        }
        else
        {
            putchar_callback(x, y, c, color);
        }
    }
}


void Terminal::new_line()
{
    add_line_to_history();
    for (size_t i { 0 }; i < width(); ++i)
    {
        set_entry_at(' ', color(), i, terminal_row);
        cur_line[i] = {' ', color()};
    }

    terminal_column = 0;
    ++terminal_row;

    redraw();
}


void Terminal::add_line_to_history()
{
    history.add(cur_line);
}


void Terminal::check_pos()
{
    if (terminal_column >= width())
    {
        terminal_column = terminal_column%width();
        add_line_to_history();
        ++terminal_row;
    }
    if (terminal_row >= height())
    {
        scroll_up();
        terminal_row = height()-1;
    }

    update_cursor();
}

void Terminal::update_cursor()
{
    if (move_cursor_callback && enabled)
    {
        move_cursor_callback(terminal_column, terminal_row+title_height, width());
    }
}

void setup_term(size_t width, size_t height, size_t history)
{
    current_term = std::make_unique<Terminal>(width, height, history);
}

Terminal &term()
{
    assert(current_term != nullptr);

    return *current_term;
}
