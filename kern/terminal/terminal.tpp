/*
terminal.tpp

Copyright (c) 10 Yann BOUCHER (yann)

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
#ifndef TERMINAL_TPP
#define TERMINAL_TPP

#include "terminal.hpp"

#include <string.h>
#include <ctype.h>

#ifdef ARCH_i686
#include "i686/pc/devices/speaker.hpp"
#include "i686/pc/serialdebug.hpp"
#include "i686/pc/bios/bda.hpp"
#endif

#include "utils/dynarray.hpp"

#include "io.hpp"
#include "halt.hpp"

#include "utils/minmax.hpp"

template<size_t Width, size_t Height, size_t MaxHistory>
TerminalImpl<Width, Height, MaxHistory>::TerminalImpl(uint16_t *term_buf)
{
    terminal_buffer = term_buf;
    clear();
}

template<size_t Width, size_t Height, size_t MaxHistory>
void TerminalImpl<Width, Height, MaxHistory>::set_color(uint8_t color)
{
    terminal_color = color;
}

template<size_t Width, size_t Height, size_t MaxHistory>
void TerminalImpl<Width, Height, MaxHistory>::put_entry_at(uint8_t c, uint8_t color, size_t x, size_t y)
{
    check_pos();
    const size_t index = y * Width + x;
    terminal_buffer[index] = vga_entry(c, color);
}

template<size_t Width, size_t Height, size_t MaxHistory>
void TerminalImpl<Width, Height, MaxHistory>::put_char(uint8_t c)
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

        put_entry_at(' ', terminal_color, terminal_column, terminal_row);

        check_pos();
    }
    else if (c == '\t')
    {
        terminal_column += 4;
    }
    else if (c == '\a')
    {
#ifdef ARCH_i686
        Speaker::beep(200);
#endif
    }
    else if (isprint(c))
    {
        put_entry_at(c, terminal_color, terminal_column, terminal_row);
        ++terminal_column;
    }

    check_pos();
}

template<size_t Width, size_t Height, size_t MaxHistory>
void TerminalImpl<Width, Height, MaxHistory>::write(const char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        put_char(data[i]);
    }
}

template<size_t Width, size_t Height, size_t MaxHistory>
void TerminalImpl<Width, Height, MaxHistory>::write_string(const char *data)
{
    write(data, strlen(data));
}

template<size_t Width, size_t Height, size_t MaxHistory>
void TerminalImpl<Width, Height, MaxHistory>::clear()
{
    memsetw(terminal_buffer, vga_entry(' ', terminal_color), Height*Width*4);
}

template<size_t Width, size_t Height, size_t MaxHistory>
void TerminalImpl<Width, Height, MaxHistory>::scroll_up()
{
    for (size_t y { 1 }; y < Height; ++y)
    {
        memcpy(terminal_buffer + (y-1)*Width, terminal_buffer + y*Width, Width*4); // copy line below
    }
    memsetw(terminal_buffer + (Height-1)*Width, vga_entry(' ', terminal_color), Width*4); // clear scrolled line
    --terminal_row;
    update_cursor();
}

template<size_t Width, size_t Height, size_t MaxHistory>
void TerminalImpl<Width, Height, MaxHistory>::push_color(uint8_t color)
{
    //serial::debug::write("%zd\n", history._front);
    old_terminal_color = terminal_color;
    set_color(color);
}

template<size_t Width, size_t Height, size_t MaxHistory>
void TerminalImpl<Width, Height, MaxHistory>::pop_color()
{

    set_color(old_terminal_color);
}

template<size_t Width, size_t Height, size_t MaxHistory>
void TerminalImpl<Width, Height, MaxHistory>::show_history(int page)
{
    if (page < 0)
    {
#ifdef ARCH_i686
        //Speaker::beep(200);
#endif
        page = 0;
    }

    if (static_cast<size_t>(page) > history.size() - Height)
    {
#ifdef ARCH_i686
        //Speaker::beep(200);
#endif
        page = history.size() - Height; // avoir un plafond, une limite
    }

    current_history_page = page;

    for (size_t i { 0 }; i < Height-1; ++i) // ignore first line where everything is typed
    {
        for (size_t j { 0 }; j < Width; ++j)
        {
            int index = history.size() - (Height-i) -page;
            if (index >= 0)
            {
                terminal_buffer[i*Width+j] = history[index][j];
            }
        }
    }
}

template<size_t Width, size_t Height, size_t MaxHistory>
void TerminalImpl<Width, Height, MaxHistory>::new_line()
{
    add_line_to_history();
    terminal_column = 0;
    ++terminal_row;
}

template<size_t Width, size_t Height, size_t MaxHistory>
void TerminalImpl<Width, Height, MaxHistory>::add_line_to_history()
{
    dynarray<uint16_t> line(Width);
    for (size_t i { 0 }; i < Width; ++i)
    {
        line[i] = terminal_buffer[terminal_row*Width + i];
    }
    history.add(line);
}

template<size_t Width, size_t Height, size_t MaxHistory>
void TerminalImpl<Width, Height, MaxHistory>::check_pos()
{
    if (terminal_column >= Width)
    {
        terminal_column = terminal_column%Width;
        add_line_to_history();
        ++terminal_row;
    }
    if (terminal_row >= Height)
    {
        scroll_up();
        terminal_row = Height-1;
    }

    update_cursor();
}

template<size_t Width, size_t Height, size_t MaxHistory>
void TerminalImpl<Width, Height, MaxHistory>::move_cursor(size_t x, size_t y)
{
#ifdef ARCH_i686
    const size_t index = y * Width + x;

    const uint16_t port_low = BDA::video_io_port();
    const uint16_t port_high = port_low + 1;

    // cursor LOW port to vga INDEX register
    outb(port_low, 0x0F);
    outb(port_high, static_cast<uint8_t>(index&0xFF));

    // cursor HIGH port to vga INDEX register
    outb(port_low, 0x0E);
    outb(port_high, static_cast<uint8_t>((index>>8)&0xFF));
#else
#endif
}

template<size_t Width, size_t Height, size_t MaxHistory>
void TerminalImpl<Width, Height, MaxHistory>::update_cursor()
{
    move_cursor(terminal_column, terminal_row);
}


#endif // TERMINAL_TPP
