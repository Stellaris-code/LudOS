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

#include "mem/memmap.hpp"

#include "utils/logging.hpp"

namespace elf
{

SymbolTable kernel_symbol_table;

SymbolTable get_symbol_table(const Elf32_Shdr *base, size_t sh_num)
{
    if (base == nullptr || sh_num == 0) return {};

    SymbolTable symbol_table;

    const char* strtable;
    if (elf::section(base, sh_num, elf::SHT_STRTAB)->sh_addr)
    {
        strtable = (const char*)Memory::mmap((void*)elf::section(base, sh_num, elf::SHT_STRTAB)->sh_addr,
                                             elf::section(base, sh_num, elf::SHT_STRTAB)->sh_size);
    }
    else
    {
        strtable = reinterpret_cast<const char*>(current_elf_file + elf::section(base, sh_num, elf::SHT_STRTAB)->sh_offset);
    }

    auto symtab = (Elf32_Shdr*)elf::section(base, sh_num, elf::SHT_SYMTAB);
    if (!symtab) return {};
    symtab->sh_addr = (Elf32_Addr)Memory::mmap((void*)symtab->sh_addr, symtab->sh_size, Memory::Read);

    std::string current_symbol_file;

    for (size_t j { 0 }; j < symtab->sh_size / symtab->sh_entsize; ++j)
    {
        auto symbol = (const Elf32_Sym*)elf::symbol(symtab, j);
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

    Memory::unmap((void*)symtab->sh_addr, symtab->sh_size);

    return symbol_table;
}

SymbolTable get_symbol_table_file(gsl::span<const uint8_t> file)
{
    const Elf32_Ehdr* hdr = reinterpret_cast<const Elf32_Ehdr*>(file.data());
    if (!elf::check_supported(hdr)) return {};

    const Elf32_Shdr* shdr = reinterpret_cast<const Elf32_Shdr*>(file.data() + hdr->e_shoff);

    elf::current_elf_file = file.data();

    return get_symbol_table(shdr, hdr->e_shnum);
}

bool has_symbol_table(const Elf32_Shdr *base, size_t sh_num)
{
    return elf::section(base, sh_num, elf::SHT_SYMTAB) != nullptr;
}


}
