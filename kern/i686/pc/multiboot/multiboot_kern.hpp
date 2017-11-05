/*
multiboot_print.hpp

Copyright (c) 24 Yann BOUCHER (yann)

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
#ifndef MULTIBOOT_KERN_HPP
#define MULTIBOOT_KERN_HPP

#include "multiboot.h"
#include <stdint.h>

#include <string.hpp>
#include <vector.hpp>

#include "elf/elf.hpp"

namespace multiboot
{

void check(uint32_t magic, const multiboot_header& mbd, const multiboot_info *mbd_info);

extern const multiboot_info_t* info;

void parse_info();
void print_info();
void parse_mem();
std::string parse_cmdline();
std::vector<multiboot_module_t> get_modules();

std::pair<const elf::Elf32_Shdr*, size_t> elf_info();

}

#endif // MULTIBOOT_KERN_HPP
