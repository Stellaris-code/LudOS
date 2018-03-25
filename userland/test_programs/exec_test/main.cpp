/*
main.cpp

Copyright (c) 25 Yann BOUCHER (yann)

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

#include <syscalls/syscall_list.hpp>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    char file[] = "/initrd/test_programs/ExecTest";
    printf("Going to execute file '%s'\n", file);

    void* waste = malloc(8000000);

    const char* empty_arr[] = {nullptr};
    char* arg_arr[argc+1];
    for (int i { 0 }; i < argc; ++i)
    {
        arg_arr[i] = (char*)malloc(strlen(argv[i]));
        strcpy(arg_arr[i], argv[i]);
        printf("Arg %d : %s\n", i, arg_arr[i]);
    }

    arg_arr[argc] = nullptr;

    execve(file, (const char**)arg_arr, empty_arr);

    return 0;
}
