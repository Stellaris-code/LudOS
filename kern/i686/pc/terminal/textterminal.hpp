/*
textterminal.hpp

Copyright (c) 17 Yann BOUCHER (yann)

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
#ifndef TEXTTERMINAL_HPP
#define TEXTTERMINAL_HPP

#include "terminal/terminal.hpp"

class TextTerminal : public Terminal
{
public:
    TextTerminal(uintptr_t fb, size_t iwidth, size_t iheight, TerminalData &data);

private:
    virtual void move_cursor(size_t x, size_t y) override;
    virtual void putchar(size_t x, size_t y, TermEntry entry) override;
    virtual void clear_line(size_t y, graphics::Color color, size_t size) override;
    virtual void draw_impl() override;
    virtual void disable_impl() override {}

private:
    uintptr_t m_fb;
};

#endif // TEXTTERMINAL_HPP
