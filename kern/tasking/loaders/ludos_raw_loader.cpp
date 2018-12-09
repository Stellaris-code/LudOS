/*
ludos_raw_loader.cpp

Copyright (c) 10 Yann BOUCHER (yann)

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

#include "ludos_raw_loader.hpp"

#include "tasking/process.hpp"
#include "tasking/process_data.hpp"

constexpr char ludos_raw_magic[] = "LUDOSBIN";
constexpr size_t ludos_raw_len = sizeof(ludos_raw_magic) - 1 + sizeof(uint32_t); // size to alloc

bool LudosRawLoader::accept(gsl::span<const uint8_t> file)
{
    if (file.size() < (int)ludos_raw_len) return false;

    return memcmp(file.data(), ludos_raw_magic, 8) == 0;
}

bool LudosRawLoader::load(Process &p)
{
    uint32_t allocated_size = *(uint32_t*)(m_file.data() + 8);

    p.load_user_code(m_file, allocated_size - 0x8000000); // TODO : name
    p.set_instruction_pointer(ludos_raw_len + 0x8000000); // skip the magic header

    return true;
}

kpp::string LudosRawLoader::file_type() const
{
    return "LudOS Raw executable";
}

ADD_PROCESS_LOADER(LudosRawLoader);
