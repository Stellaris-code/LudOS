/*
cowsay.cpp

Copyright (c) 03 Yann BOUCHER (yann)

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

#include "cowsay.hpp"

#include <algorithm.hpp>

#include <stdio.h>

#include "utils/stlutils.hpp"

void print_cow()
{
    puts("        \\   ^__^");
    puts("         \\  (oo)\\_______");
    puts( "            (__)\\       )\\/\\");
    puts(  "                ||----w |");
    puts(  "                ||     ||");
}

void cowsay(std::string str, size_t cowsay_width)
{
    assert(cowsay_width);

    // remove newlines
    str.erase(std::remove_if(str.begin(), str.end(), [](char c){return c == '\n';}), str.end());
    str = trim_right(str);

    size_t lines = str.size()/cowsay_width + (str.size()%cowsay_width?1:0);

    const size_t largest_width = std::min(str.size(), cowsay_width);

    kprintf(" ");
    for (size_t i { 0 }; i <= largest_width; ++i)
    {
        putchar('_');
    }
    putchar('_');
    putchar('\n');

    for (size_t i { 0 }; i < lines; ++i)
    {
        size_t width = std::min(str.size(), cowsay_width);

        putchar(lines == 1 ? '<' : i == 0 ? '/' : i == lines-1 ? '\\' : '|');

        kprintf(" %s ", str.substr(0, width).c_str());

        str.erase(0, width);

        while (width++ < largest_width) putchar(' ');

        putchar(lines == 1 ? '>' : i == 0 ? '\\' : i == lines-1 ? '/' : '|');
        putchar('\n');
    }

    kprintf(" ");
    for (size_t i { 0 }; i <= largest_width; ++i)
    {
        putchar('-');
    }
    putchar('-');
    putchar('\n');

    print_cow();
}
