/*
main.cpp

Copyright (c) 11 Yann BOUCHER (yann)

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

char global_str[] = "Bonjour depuis le mode utilisateur !\n";

char buffer[512];

//#include "syscall.h"

void panic(char* buf, int dummy)
{
    asm volatile ("leal (%1), %%ecx\n"
                  "mov %2, %%edx\n"
                  "mov %0, %%ebx\n"
                  "mov $2, %%eax\n"
                  "int $0x70\n" // sys_panic
                  :: "m"(buf), "g"((unsigned int)&buf + sizeof(buf)), "g"(dummy));
}

int main()
{

    char local_str[] = "Bonjour depuis le mode utilisateur !\n";

    asm volatile ("leal %0, %%ebx\n"
                  "mov $1, %%eax\n"
                  "int $0x70\n" // sys_print_debug
                  "\n" ::"m"(local_str));

    asm volatile ("leal %0, %%ebx\n"
                  "mov $512, %%ecx\n"
                  "mov $183, %%eax\n"
                  "int $0x80\n" // linux, sys_getcwd
                  "\n" ::"m"(buffer));

    panic(buffer, 0xCAFEBABE);

    while (true){}
    return 0;
}
