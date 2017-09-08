/*
paging.cpp

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

#include "paging.hpp"

#include "utils/stdint.h"
#include "utils/addr.hpp"

#include "halt.hpp"
#include "panic.hpp"
#include "utils/logging.hpp"

#include "meminfo.hpp"
#include "multiboot/multiboot.h"

extern "C" const uint32_t kernel_physical_end;
extern "C" const uint32_t page_directory;
extern "C" const uint32_t page_table_0;

void Paging::init()
{
    uint32_t* pd0 = (uint32_t*)&page_directory;
    pd0[0] = (uint32_t)&page_table_0;
    pd0[0] |= 3;
    for (size_t i = 1; i < 1024; i++)
    {
        pd0[i] = 0;
    }


    /* Création de la Page Table[0] */
    uint32_t* pt0 = (uint32_t*)&page_table_0;
    uint32_t page_addr = 0;
    for (size_t i = 0; i < 1024; i++)
    {
        pt0[i] = page_addr;
        pt0[i] |= 3;
        page_addr += 0x1000;
    }

    // must use inline assembly, cannot call external function as that would mess up the stack
    asm("   mov %0, %%eax    \n \
            mov %%eax, %%cr3 \n \
            mov %%cr0, %%eax \n \
            or $0x80000001, %%eax     \n \
            mov %%eax, %%cr0" :: "m"(pd0));

    log("Paging initialized\n");

    //    init_memchunks();
    //    //memset(mem_bitmap, 0, size(mem_bitmap) * sizeof(uint8_t));

    //    // Hardware
    //    for (uintptr_t pg = page(kernel_base); pg < page(kernel_base+0x100000); ++pg)
    //    {
    //        set_page_frame_used(pg);
    //    }

    //    // Kernel
    //    for (uintptr_t pg = page(kernel_base+0x100000); pg < page((uintptr_t)&kernel_virtual_end); ++pg)
    //    {
    //        set_page_frame_used(pg);
    //    }
}






//uint8_t *Paging::alloc_page_frame(size_t number)
//{
//    size_t chunks = Meminfo::free_frames();
//    for (size_t i { 0 }; i < chunks; ++i)
//    {
//        auto* chunk = Meminfo::frame(i);
//        uint8_t* bitmap = (uint8_t*)chunk;

//        size_t counter { 0 };
//        size_t page { 0 };

//        for (size_t byte { 0 }; byte < chunk->len/page_size/8; ++byte)
//        {
//            if (bitmap[byte] != 0xFF)
//            {
//                for (uint8_t bit = 0; bit < 8; bit++)
//                {
//                    if (!(bitmap[byte] & (1 << bit)))
//                    {
//                        if (counter == 0)
//                        {
//                            page = 8 * byte + bit;
//                        }
//                        ++counter;
//                        if (counter >= number)
//                        {
//                            return (uint8_t*)phys(page * page_size + chunk->len/page_size/8);
//                        }
//                    }
//                    else
//                    {
//                        counter = 0;
//                    }
//                }
//            }
//        }
//    }

//    err("Cannot allocate %d pages !\n", number);

//    return nullptr;
//}

//bool Paging::release_page_frame(uintptr_t p_addr, size_t number)
//{
//    size_t chunks = Meminfo::free_frames();
//    for (size_t i { 0 }; i < chunks; ++i)
//    {
//        auto* chunk = Meminfo::frame(i);
//        // corresponding chunk
//        if (p_addr >= phys(chunk->addr) && p_addr < phys(chunk->addr + chunk->size))
//        {
//            uint8_t* bitmap = (uint8_t*)phys(chunk->addr);
//            for (size_t i { 0 }; i < number; ++i)
//            {
//                bitmap[(p_addr/page_size)/8] &= ~(1 << ((p_addr/page_size)%8));
//            }
//            return true;
//        }
//    }

//    err("Cannot release %d pages at address %p !\n", number, p_addr);

//    return false;
//}

//void Paging::init_memchunks()
//{
//    size_t chunks = Meminfo::free_frames();
//    for (size_t i { 0 }; i < chunks; ++i)
//    {
//        init_memchunk(Meminfo::frame(i));
//    }
//}

//void Paging::init_memchunk(multiboot_memory_map_t *chunk)
//{
//    const size_t size = chunk->len/page_size/8;

//    uint8_t* bitmap = (uint8_t*)phys(chunk->addr);

//    memset(bitmap, 0, size*sizeof(uint8_t));
//}
