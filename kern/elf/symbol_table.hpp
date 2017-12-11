/*
symbol_table.hpp

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
#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

#include <stdint.h>

#include <unordered_map.hpp>
#include <string.hpp>
#include <optional.hpp>
#include <vector.hpp>

#include "elf.hpp"

namespace elf
{

struct SymbolInfo
{
    std::string name;
    std::string file;
    uintptr_t offset;
};

struct SymbolTable
{
    std::optional<SymbolInfo> get_function(uintptr_t addr)
    {
        while (addr-- > 0)
        {
            if (auto it = table.find(addr); it != table.end())
            {
                return it->second;
            }
        }

        return {};
    }

    std::unordered_map<uintptr_t, SymbolInfo> table;
};

SymbolTable get_symbol_table(const Elf32_Shdr* base, size_t sh_num);

SymbolTable get_symbol_table_file(const std::vector<uint8_t>& file);

extern SymbolTable kernel_symbol_table;

}

#endif // SYMBOL_TABLE_HPP
