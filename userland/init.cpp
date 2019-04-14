/*
init.cpp

Copyright (c) 18 Yann BOUCHER (yann)

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

#include <stdio.h>

#include <syscalls/syscall_list.hpp>

extern "C" void pthread_init();
extern "C" void pthread_terminate();

char print_buffer[1024];
size_t buf_counter;

void flush_buffer()
{
    write(1, print_buffer, buf_counter);
    buf_counter = 0;
}

void write_callback(void*, char c)
{
    print_buffer[buf_counter++] = c;
    if (c == '\n' || buf_counter == sizeof(print_buffer))
        flush_buffer();
}

void __attribute__((constructor)) libc_init()
{
    // initialize the FPU
    asm volatile ("fninit\n");

    init_printf(nullptr, write_callback);
    pthread_init();
}

void __attribute__((destructor)) libc_exit()
{
    pthread_terminate();
}
