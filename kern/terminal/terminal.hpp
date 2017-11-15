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

#include <stdint.h>
#include "utils/addr.hpp"
#include "stack.hpp"
#include "graphics/color.hpp"
#include "unicode/utf8decoder.hpp"

#include <functional.hpp>
#include <memory.hpp>
#include <string.hpp>

#include "historybuffer.hpp"

class Terminal
{
public:
    Terminal(size_t iwidth, size_t iheight, size_t imax_history = 5);

public:
    void put_entry_at(char32_t c, TermEntry color, size_t x, size_t y);
    void put_char(char32_t c);
    void write(const char* data, size_t size);
    void write_string(const char* data);
    void clear();
    void clear(TermEntry color);
    void scroll_up();
    void push_color(TermEntry color);
    void pop_color();
    void show_history(int page);
    size_t current_history() const { return current_history_page; }
    void scroll_history(int scroll) { show_history(current_history()+scroll); }
    TermEntry color() const;

    size_t width() const { return _width; }
    size_t height() const { return _height-title_height; }
    size_t history_lines() const { return max_history; }

    void set_title(std::string str, TermEntry color);
    void set_title(std::string str);

    void enable()
    {
        enabled = true;
        set_title(title_text, text_color);
        redraw();
    }
    void disable() { enabled = false; }

    void redraw();

private:
    void set_entry_at(char32_t c, TermEntry color, size_t x, size_t y, bool absolute = false);
    void new_line();
    void add_line_to_history();
    void check_pos();
    void update_cursor();

public:
    std::function<void(size_t x, size_t y, size_t width)> move_cursor_callback;
    std::function<void(size_t ms)> beep_callback;
    std::function<void(size_t x, size_t y, char32_t c, TermEntry color)> putchar_callback;
    std::function<void()> redraw_callback;

private:
    size_t terminal_row { 0 };
    size_t terminal_column { 0 };
    size_t title_height { 1 };

    bool enabled { true };

    std::vector<HistoryBuffer::Entry> cur_line;

    std::stack<TermEntry> color_stack;

    const size_t _width;
    const size_t _height;
    const size_t max_history;

    std::string title_text;
    TermEntry text_color;

    UTF8Decoder decoder;

    HistoryBuffer history;
    uint8_t current_history_page { 0 };
};

void setup_term(size_t width, size_t height, size_t history);

Terminal &term();

#endif // TERMINAL_HPP
