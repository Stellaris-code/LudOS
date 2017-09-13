/*
kmain.cpp

Copyright (c) 23 Yann BOUCHER (yann)

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

// TODO : ACPI
// TODO : disque
// TODO : ext2
// TODO : scrolling terminal char par char
// TODO : system calls
// TODO : user mode
// TODO : POC calculatrice
// TODO : utiliser une vraie implémentation de printf (newlib ?)
// TODO : mettre le code d'intialization arch specific ailleurs dans un fichier à part

// FIXME : bug si clavier utilisé avant init

#ifndef __cplusplus
#error Must be compiler using C++ !
#endif

#ifndef NDEBUG
#define DEBUG
#endif

#include "utils/addr.hpp"
#include "utils/bitarray.hpp"
#include "utils/dynarray.hpp"

#include "timer.hpp"

#include "ext/liballoc/liballoc.h"

#include "utils/logging.hpp"
#include "greet.hpp"
#include "halt.hpp"

#ifdef ARCH_i686
#include "i686/pc/init.hpp"
#endif

extern "C" uint32_t kernel_physical_end;

#ifdef ARCH_i686
extern "C"
void kmain(uint32_t magic, const multiboot_info_t* mbd_info)
#else
void kmain()
#endif
{
#ifdef ARCH_i686
    i686::pc::init(magic, mbd_info);
#endif

    greet();

    char* ptr = (char*)kmalloc(256*1000*1000);
    liballoc_dump();
    ptr[4] = 'c';
    kfree(ptr);
    liballoc_dump();

    while (1)
    {
        //        log("dd\n");
        NOP();
    }
}
