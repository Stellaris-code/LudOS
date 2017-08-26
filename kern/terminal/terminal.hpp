/*
terminal.hpp

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
#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include "utils/stdint.h"
#include "utils/addr.hpp"
#include "vga.hpp"


namespace detail
{

class TerminalImpl
{
public:
    TerminalImpl();

public:
    static constexpr size_t vga_width = 80;
    static constexpr size_t vga_height = 25;

public:
    void set_color(uint8_t color);
    void put_entry_at(char c, uint8_t color, size_t x, size_t y);
    void put_char(char c);
    void write(const char* data, size_t size);
    void write_string(const char* data);
    void clear();
    void scroll_up();

private:
    void check_pos();
    void move_cursor(size_t x, size_t y);
    void update_cursor();

private:
    size_t terminal_row { 0 };
    size_t terminal_column { 0 };
    uint8_t terminal_color { vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK) };
    uint16_t* terminal_buffer { reinterpret_cast<uint16_t*>(phys(0xB8000)) };
};

}

class Terminal
{
public:
    Terminal() = delete;

public:
    static void set_color(uint8_t color) { impl.set_color(color); }
    static void put_entry_at(char c, uint8_t color, size_t x, size_t y) { impl.put_entry_at(c, color, x, y); }
    static void put_char(char c) { impl.put_char(c); };
    static void write(const char* data, size_t size) { impl.write(data, size); };
    static void write_string(const char* data) { impl.write_string(data); }
    static void clear() { impl.clear(); }
    static void scroll_up() { impl.scroll_up(); }


private:
    static inline detail::TerminalImpl impl;
};

#endif // TERMINAL_HPP
