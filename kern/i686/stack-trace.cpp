/*
stack-trace.cpp

Copyright (c) 31 Yann BOUCHER (yann)

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

#include "stack-trace.hpp"

#include "utils/logging.hpp"
#include "mem/memmap.hpp"

struct [[gnu::packed]] stack_frame
{
    stack_frame* previous;
    uintptr_t return_addr;
};

std::vector<uintptr_t> trace_stack(void *addr, size_t frames)
{
    std::vector<uintptr_t> trace;

    stack_frame* fp = (stack_frame*)(addr ?: __builtin_frame_address(0));

    for(size_t i = 0; (frames != 0 ? i < frames : true) && fp && Memory::is_mapped(fp) && fp->return_addr; i++)
    {
        trace.emplace_back(reinterpret_cast<uintptr_t>(fp->return_addr));
        fp = fp->previous;
    }

    return trace;
}
