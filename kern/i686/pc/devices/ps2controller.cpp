/*
ps2controller.cpp

Copyright (c) 30 Yann BOUCHER (yann)

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

#include "ps2controller.hpp"

#include "io.hpp"

void PS2Controller::send_command(uint8_t command, bool poll)
{
    if (poll) poll_obf();
    outb(CommandPort, command);
}

void PS2Controller::poll_ibf()
{
    size_t timeout = poll_timeout;
    while (--timeout && !(inb(CommandPort) & 0x2));
}

void PS2Controller::poll_obf()
{
    size_t timeout = poll_timeout;
    while (--timeout && (inb(CommandPort) & 0x1) == 1);
}
