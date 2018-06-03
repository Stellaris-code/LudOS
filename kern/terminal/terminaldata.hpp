/*
terminaldata.hpp

Copyright (c) 15 Yann BOUCHER (yann)

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
#ifndef TERMINALDATA_HPP
#define TERMINALDATA_HPP

#include <stdint.h>

#include <vector.hpp>
#include <stack.hpp>

#include <kstring/kstring.hpp>

#include "utils/circularbuffer.hpp"

#include "termentry.hpp"

#include "utils/logging.hpp"

class TerminalData
{
public:
    TerminalData(size_t history_size)
        : m_buffer(history_size)
    {
        push_color(ColorPair{0xaaaaaa, 0x000000});
    }

    void add_line(const std::vector<TermEntry>& entry);

    std::vector<std::vector<std::reference_wrapper<const TermEntry>> > get_screen(size_t width, size_t height, size_t offset) const;

    void set_entry(size_t x, size_t y, TermEntry entry);

    void clear();

    size_t lines() const
    {
        return m_buffer.size();
    }

    void push_color(ColorPair color)
    {
        color_stack.push(color);
    }

    void pop_color()
    {
        color_stack.pop();
    }

    ColorPair color() const
    {
        return color_stack.top();
    }

public:
    kpp::u32string title_str;
    ColorPair title_color;
    size_t title_height { 1 };
    std::stack<ColorPair> color_stack;

private:
    CircularBuffer<std::vector<TermEntry>> m_buffer;
};

#endif // TERMINALDATA_HPP
