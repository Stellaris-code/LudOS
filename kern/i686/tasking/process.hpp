/*
process.hpp

Copyright (c) 13 Yann BOUCHER (yann)

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
#ifndef i686_PROCESS_HPP
#define i686_PROCESS_HPP

#include "tasking/process.hpp"

#include "utils/aligned_vector.hpp"
#include "i686/mem/paging.hpp"
#include "i686/fpu/fpu.hpp"
#include "i686/cpu/registers.hpp"

struct Process::ArchSpecificData
{
    aligned_vector<uint8_t, Paging::page_size> stack;
    aligned_vector<uint8_t, Paging::page_size> code;
    registers regs;
    FPUState fpu_state;
};

#endif // i686_PROCESS_HPP
