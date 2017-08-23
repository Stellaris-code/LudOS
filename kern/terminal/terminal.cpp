/*
terminal.cpp

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

detail::TerminalImpl::TerminalImpl()
{
    memsetw(terminal_buffer, vga_entry(' ', terminal_color), vga_height*vga_width);
}

void detail::TerminalImpl::set_color(uint8_t color)
{
    terminal_color = color;
}

void detail::TerminalImpl::put_entry_at(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * vga_width + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void detail::TerminalImpl::put_char(char c)
{
    if (c == '\n')
    {
        terminal_column = 0;
        ++terminal_row;
    }
    else if (c == '\r')
    {
        terminal_column = 0;
    }
    else if (c == '\b')
    {
        // TODO : backspace
    }
    else if (c == '\t')
    {
        terminal_column += 4;
        if (terminal_column >= vga_width) terminal_column = vga_width-1;
    }
    else if (c == '\a')
    {
        // TODO : bell !
    }
    else
    {
        put_entry_at(c, terminal_color, terminal_column, terminal_row);
        ++terminal_column;
    }

    check_pos();
}

void detail::TerminalImpl::write(const char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        put_char(data[i]);
    }
}

void detail::TerminalImpl::write_string(const char *data)
{
    write(data, strlen(data));
}

void detail::TerminalImpl::check_pos()
{
    if (terminal_column >= vga_width)
    {
        terminal_column = 0;
        ++terminal_row;
    }
    if (terminal_row >= vga_height)
    {
        terminal_row = 0;
    }
}
