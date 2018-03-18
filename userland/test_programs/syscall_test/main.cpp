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

#include <stdint.h>

#include <syscalls/syscall_list.hpp>

#include <string.hpp>
#include <liballoc/liballoc.h>

void __attribute__((constructor)) init()
{
    common_syscall(0, 2, "bouh!\n");
}

void __attribute__((constructor)) init2()
{
    common_syscall(0, 2, "bouh2!\n");
}

void __attribute__((destructor)) tini()
{
    common_syscall(0, 2, "goodbye!\n");
}

int main(int a, char* argv[])
{
    //std::string std_str = "Bonjour depuis std::string !\n";

    char buffer[512];

    char local_str[] = "Arguments :\n";

    char* allocated_str = (char*)malloc(60);
    //char* allocated_str = (char*)alloc_pages(1);

    strcpy(allocated_str, "Bonjour depuis malloc!\n");

    for (size_t i { 0 }; i < a; ++i)
    {
        print_debug(argv[i]);
        print_debug("\n");
    }

    print_debug(allocated_str);

    print_serial(local_str);

    getcwd(buffer, 512);

    char msg[] = "Pwd :           \n";
    msg[6] = buffer[0];

    print_debug(msg);

    char pid_msg[] = "PID :  \n";
    pid_msg[6] = '0' + getpid();

    print_debug(pid_msg);

    free(allocated_str);
    //free_pages((uintptr_t)allocated_str, 1);
    return 0;
}
