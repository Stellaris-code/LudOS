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

#include "io.hpp"
#include "halt.hpp"

Terminal* term;

Terminal::Terminal(size_t iwidth, size_t iheight, size_t imax_history)
    : _width(iwidth), _height(iheight), max_history(imax_history),
      history(width(), height()*max_history)
{
    push_color(TermEntry{0xaaaaaa, 0});
}


void Terminal::put_entry_at(uint8_t c, TermEntry color, size_t x, size_t y)
{
    check_pos();
    putchar_callback(x, y, c, color);
}


void Terminal::put_char(uint8_t c)
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
        put_char(data[i]);
    }
}


void Terminal::write_string(const char *data)
{
    write(data, strlen(data));
}


void Terminal::clear()
{
    for (size_t i { 0 }; i < width(); ++i)
    {
        for (size_t j { 0 }; j < height(); ++j)
        {
            putchar_callback(i, j, ' ', color());
        }
    }
}


void Terminal::scroll_up()
{
//    for (size_t y { 1 }; y < height; ++y)
//    {
//        memcpy(terminal_buffer + (y-1)*width, terminal_buffer + y*width, width*4); // copy line below
//    }
//    memsetw(terminal_buffer + (height-1)*width, vga_entry(' ', color()), width*4); // clear scrolled line
//    --terminal_row;
//    update_cursor();
//    new_line();
//    scroll_history(-1);
//    add_line_to_history();
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
        if (beep_callback)
        {
            beep_callback(200);
        }
        page = 0;
    }

    if (static_cast<size_t>(page) > history.size() - height())
    {
        if (beep_callback)
        {
            beep_callback(200);
        }
        page = history.size() - height(); // avoir un plafond, une limite

    }

    current_history_page = page;

    for (size_t i { 0 }; i < height()-1; ++i) // ignore first line where everything is typed
    {
        for (size_t j { 0 }; j < width(); ++j)
        {
            int index = history.size() - (height()-i) -page;
            if (index >= 0)
            {
                terminal_buffer[i*width()+j] = history.get_char(j, index);
            }
        }
    }
}

TermEntry Terminal::color() const
{
    return color_stack.top();
}


void Terminal::new_line()
{
    add_line_to_history();
    for (size_t i { 0 }; i < width(); ++i)
    {
        putchar_callback(i, terminal_row, ' ', color());
    }

    terminal_column = 0;
    ++terminal_row;
}


void Terminal::add_line_to_history()
{
    std::vector<uint16_t> line(width());
    for (size_t i { 0 }; i < width(); ++i)
    {
        line[i] = terminal_buffer[terminal_row*width() + i];
    }
    history.add(line);
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
    if (move_cursor_callback)
    {
        move_cursor_callback(terminal_column, terminal_row, width());
    }
}

