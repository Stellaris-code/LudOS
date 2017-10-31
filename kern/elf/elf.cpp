/*
elf.cpp

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

#include "elf/elf.hpp"

#include "utils/logging.hpp"

namespace elf
{

const char *str_table(const Elf32_Shdr *hdr, size_t strtableidx)
{
    return reinterpret_cast<const char*>(hdr[strtableidx].sh_addr);
}

const char *lookup_str(const char *strtable, size_t offset)
{
    return strtable + offset;
}

const Elf32_Sym *symbol(const Elf32_Shdr *symtab, size_t num)
{
    size_t symtab_entries = symtab->sh_size / symtab->sh_entsize;
    if(num >= symtab_entries)
    {
        warn("Symbol Index out of Range (%d:%u).\n", symtab, num);
        return nullptr;
    }

    return &((const Elf32_Sym*)symtab->sh_addr)[num];
}

const Elf32_Shdr *section(const Elf32_Shdr *base, size_t size, size_t type)
{
    for (size_t i { 0 }; i < size; ++i)
    {
        if (base[i].sh_type == type)
        {
            return base + i;
        }
    }

    return nullptr;
}

}
