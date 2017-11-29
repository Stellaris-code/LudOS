/*
terminal.hpp

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
#ifndef TERMINAL_HPP
#define TERMINAL_HPP

#include <stdint.h>

#include "graphics/color.hpp"
#include "unicode/utf8decoder.hpp"

#include <string.hpp>
#include <stack.hpp>
#include <type_traits.hpp>
#include <memory.hpp>

#include "utils/circularbuffer.hpp"

#include "terminaldata.hpp"
#include "termentry.hpp"

struct TermInputEvent
{
    std::string line;
};

class Terminal
{
public:
    Terminal(size_t iwidth, size_t iheight, TerminalData& data);

public:
    void put_char(char32_t c);

    void add_input(char32_t c);
    void clear_input();
    void set_input();

    void write(const char* data, size_t size);
    void write_string(const char* data);

    void clear();
    void clear(ColorPair color);

    void show_history(int page);
    size_t current_history() const;
    void scroll_history(int scroll);
    void scroll_up();
    void scroll_bottom();

    void resize(size_t iwidth, size_t iheight);

    size_t width() const;
    size_t height() const;
    size_t true_height() const;

    void set_title(std::u32string str, ColorPair color);
    void set_title(std::u32string str);

    void enable() { m_enabled = true; };
    void disable()
    {
        m_enabled = false;
        force_redraw();
    };

    void set_scrolling(bool enabled)
    {
        m_scrolling = enabled;
    }

    void force_redraw();
    void force_redraw_input();
    void draw();

private:
    void set_entry_at(TermEntry entry, size_t x, size_t y, bool absolute = false);
    void new_line();
    void add_line_to_history();
    void check_pos();
    void update_cursor();
    void reset();

private:
    virtual void move_cursor(size_t x, size_t y) = 0;
    virtual void beep(size_t ms) = 0;
    virtual void clear_line(size_t y, graphics::Color color) = 0;
    virtual void putchar(size_t x, size_t y, TermEntry entry) = 0;
    virtual void draw_impl() = 0;

private:
    size_t m_cursor_x { 0 };
    size_t m_cursor_y { 0 };

    size_t m_width { 0 };
    size_t m_height { 0 };
    size_t m_current_history_page { 0 };

    bool m_enabled { true };
    mutable bool m_dirty { true };
    bool m_scrolling { true };
    bool m_line_is_input { false };
    size_t m_input_off { 0 };

    std::vector<TermEntry> m_cur_line;

    UTF8Decoder m_decoder;

    TerminalData& m_data;
};

Terminal& term();

TerminalData& term_data();

template <typename T, typename... Args>
void create_term(Args&&... args)
{
    static_assert(std::is_base_of_v<Terminal, T>);

    extern std::unique_ptr<Terminal> current_term;

    current_term = std::make_unique<T>(std::forward<Args>(args)...);
    current_term->set_title(term_data().title_str, term_data().title_color);
    current_term->scroll_bottom();
}

void reset_term();

#endif // TERMINAL_HPP
