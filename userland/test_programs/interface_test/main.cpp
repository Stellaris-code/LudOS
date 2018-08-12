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

#include <sys/fnctl.h>
#include <sys/interface_list.h>
#include <errno.h>
#include <stdio.h>

int main(int argc, char* argv[])
{
    int fd = open("/proc/interface_test", O_RDONLY, 0);
    if (fd == -1)
    {
        perror("open()");
        return 1;
    }

    itest interface;
    int ret = get_interface(fd, ITEST_ID, &interface);
    if (ret == -1)
    {
        perror("get_interface()");
        return 2;
    }

    fprintf(stderr, "address is : %p\n", interface.test);

    ret = interface.test("working!");

    printf("The call returned %d\n", ret);

    while (true) {}

    return 0;
}
