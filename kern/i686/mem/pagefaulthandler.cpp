/*
pagefaulthandler.cpp

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

#include "paging.hpp"

#include "panic.hpp"

#include <string.hpp>

#include "utils/stlutils.hpp"

bool Paging::page_fault_handler(const registers *regs)
{
    panic_regs = regs;

    std::string message;

    if (regs->err_code & 2)
    {
        message += "unprivileged ";
    }

    if (regs->err_code & 1)
    {
        message += "write ";
    }
    else if (regs->err_code & 8)
    {
        message += "instruction fetch ";
    }
    else
    {
        message += "read ";
    }

    if (regs->err_code & 4)
    {
        message += "in a reserved bit ";
    }

    message += "at ";
    char buf[16];
    ksnprintf(buf, 16, "0x%x ", cr2());
    message += trim_zstr(buf);

    if (regs->err_code & 0)
    {
        message += "(page exists)";
    }
    else
    {
        message += "(page doesn't exist)";
    }

    message[0] = toupper(message[0]); // Capitalize

    if (cr2() == 0)
    {
        panic("Null pointer access : %s\n", message.c_str());
    }
    else
    {
        panic("Page fault : %s\n", message.c_str());
    }

    return false;
}
