/*
symbol_table.cpp

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

#include "symbol_table.hpp"

namespace elf
{

SymbolTable kernel_symbol_table;

SymbolTable get_symbol_table(const Elf32_Shdr *base, size_t sh_num)
{
    if (base == nullptr || sh_num == 0) return {};

    SymbolTable symbol_table;

    auto strtable = reinterpret_cast<const char*>(elf::section(base, sh_num, elf::SHT_STRTAB)->sh_addr);

    auto symtab = elf::section(base, sh_num, elf::SHT_SYMTAB);

    std::string current_symbol_file;

    for (size_t j { 0 }; j < symtab->sh_size / symtab->sh_entsize; ++j)
    {
        auto symbol = elf::symbol(symtab, j);
        if (symbol->st_name)
        {
            if (ELF32_ST_TYPE(symbol->st_info) == elf::STT_FILE)
            {
                current_symbol_file = elf::lookup_str(strtable, symbol->st_name);
            }
            else if (symbol->st_value)
            {
                symbol_table.table[symbol->st_value] = {elf::lookup_str(strtable, symbol->st_name), current_symbol_file, symbol->st_value};
            }
        }
    }

    return symbol_table;
}


}
