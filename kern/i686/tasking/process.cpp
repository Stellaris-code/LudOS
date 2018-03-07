/*
process.cpp

Copyright (c) 05 Yann BOUCHER (yann)

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

#include "tasking/process.hpp"

#include <string.hpp>

#include "i686/gdt/gdt.hpp"
#include "i686/mem/paging.hpp"
#include "i686/interrupts/interrupts.hpp"

#include "utils/membuffer.hpp"

extern "C" void enter_ring3(uint32_t esp, uint32_t eip);

namespace tasking
{

struct Process::ProcessData
{
    std::string name;
    uint32_t id;
    MemBuffer stack;
    MemBuffer code;
};

Process::Process(const std::string &name, void *address, size_t size)
    : m_data(new ProcessData)
{
    m_data->name = name;
    m_data->id = 0;
    m_data->stack.resize(0x1000);
    m_data->code.resize(size);
    std::copy((uint8_t*)address, (uint8_t*)address + size, m_data->code.begin());
}

void Process::execute()
{
    log_serial("0x%x/0x%x\n", (uint32_t)m_data->stack.data(),
               (uint32_t)m_data->code.data());

    enter_ring3((uint32_t)m_data->stack.data(), (uint32_t)m_data->code.data());
}

}
