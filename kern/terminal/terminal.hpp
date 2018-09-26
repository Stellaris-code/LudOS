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

#include <stack.hpp>
#include <type_traits.hpp>
#include <memory.hpp>
#include <vector.hpp>

#include "utils/circularbuffer.hpp"

#include "terminaldata.hpp"
#include "termentry.hpp"

struct TermInputEvent
{
    kpp::string line;
};

class Terminal
{
public:
    Terminal(size_t iwidth, size_t iheight, TerminalData& data);
    virtual ~Terminal() = default;

public:
    void put_char(char32_t c);

    void add_input(char32_t c);
    void clear_input();
    void set_input(const kpp::string &str);
    void switch_to_input();
    kpp::string input() const;
    void set_input_color(size_t pos, size_t sz, ColorPair color);

    void write(const char* data, size_t size);
    void write_string(const char* data);

    void forward_delete();

    void clear();
    void clear(ColorPair color);

    void move_cursor(int offset);

    void show_history(int page);
    size_t current_history() const;
    void scroll_history(int scroll);
    void scroll_up();
    void scroll_bottom();

    void resize(size_t iwidth, size_t iheight);

    size_t width() const;
    size_t height() const;
    size_t true_height() const;

    void set_title(kpp::u32string str, ColorPair color);
    void set_title(kpp::u32string str);

    void set_accept_input(bool val);

    void enable() { m_enabled = true; };
    void disable()
    {
        m_enabled = false;
        disable_impl();
    };
    bool enabled() const { return m_enabled; }

    void set_scrolling(bool enabled)
    {
        m_scrolling = enabled;
    }

    void force_redraw();
    void force_redraw_input();
    void draw();

public:
    size_t tab_size { 4 };

private:
    void set_entry_at(TermEntry entry, size_t x, size_t y, bool absolute = false);
    void clear_line_before_write(size_t y, graphics::Color color, size_t size);
    void new_line();
    void add_line_to_history();
    void check_pos();
    void update_cursor();
    void reset();
    void process_escape_code();

private:
    virtual void move_cursor(size_t x, size_t y) = 0;
    virtual void clear_line(size_t y, graphics::Color color, size_t size) = 0;
    virtual void clear_screen(graphics::Color color);
    virtual void putchar(size_t x, size_t y, TermEntry entry) = 0;
    virtual void draw_impl() = 0;
    virtual void disable_impl() = 0;

protected:
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
    bool m_accept_input { false };

    bool m_escape_code { false };
    bool m_expecting_bracket { false };
    kpp::string m_escape_sequence;

    std::vector<TermEntry> m_cur_line;
    std::vector<size_t> m_dirty_width_per_line;

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
    if (current_term) current_term->disable();
    current_term = std::make_unique<T>(std::forward<Args>(args)...);
    current_term->set_title(term_data().title_str, term_data().title_color);
    current_term->scroll_bottom();
    current_term->force_redraw();
}

void reset_term();

#endif // TERMINAL_HPP
