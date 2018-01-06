/*
exception_support.cpp

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

#include "cpp_runtime/exception_support.hpp"

#include "libunwind.h"

#include "utils/defs.hpp"
#include "utils/logging.hpp"
#include "elf/elf.hpp"

#include "elf/kernel_binary.hpp"


extern "C" unsigned int _dl_osversion;
/* Platform name.  */
extern "C" const char *_dl_platform;

/* Cached value of `getpagesize ()'.  */
extern "C" size_t _dl_pagesize;

extern "C" void _dl_non_dynamic_init();

void init_exceptions()
{
    _dl_platform = "LudOS";
    _dl_osversion = LUDOS_MAJOR;
    _dl_pagesize = 0x1000;

    _dl_non_dynamic_init();

//    const elf::Elf32_Ehdr* elf = elf::kernel_binary();

//    for (size_t i { 0 }; i < elf->e_phnum; ++i)
//    {
//        log(Info, "Type : 0x%x\n", elf::program_header(elf, i)->p_type);
//    }
}
