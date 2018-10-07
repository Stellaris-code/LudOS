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

#include <stdio.h>
#include <sys/fnctl.h>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        fprintf(stdout, "cat needs one argument\n");
        return 1;
    }

    const char* path = argv[1];

    int fd = open(path, O_RDONLY, 0);
    if (fd < 0)
    {
        perror("open");
        return 0;
    }

    char buf[0x1000];
    if (read(fd, buf, sizeof(buf)) < 0)
    {
        perror("read");
        return 0;
    }

    printf("%s\n", buf);

    return 0;
}
