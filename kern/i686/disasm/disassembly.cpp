/*
disasm.cpp

Copyright (c) 12 Yann BOUCHER (yann)

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

#include "dissasembly.hpp"

#include "libdis.h"

DisasmInfo get_disasm(uint8_t* ptr)
{
    x86_init(opt_none, nullptr, nullptr);

    x86_insn_t instr;

    size_t size = x86_disasm(ptr, 16, 0, 0, &instr);

    if (!size)
    {
        return {"invalid instr", {0}, 1};
    }

    char line[32];
    x86_format_insn(&instr, line, sizeof(line), intel_syntax);

    x86_cleanup();

    return {kpp::string(line), {instr.bytes, instr.bytes + instr.size}, size};
}
