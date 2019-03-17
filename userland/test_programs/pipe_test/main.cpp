/*
main.cpp

Copyright (c) 28 Yann BOUCHER (yann)

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

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include <sys/fnctl.h>

int main()
{
    int fd[2];
#if 0
    pipe(fd);
#else
    fd[0] = open("/home/testfifo", O_RDONLY, 0);
    fd[1] = open("/home/testfifo", O_WRONLY, 0);
#endif

    int ret = fork();
    if (ret < 0)
    {
        printf("Error : %s\n", strerror(errno));
        return 0;
    }
    else if (ret == 0)
    {
        // Child

        close(fd[0]); // close reading end

        char write_buf[100];
        strcpy(write_buf, "bonjour, c'est un test !\n");
        write(fd[1], write_buf, 10);
        write(fd[1], write_buf + 10, 90);

        exit(0);
    }
    else
    {
        // Parent

        close(fd[1]); // close writing end

        char read_buf[100];
        read(fd[0], read_buf, 10);
        read(fd[0], read_buf + 10, 90);

        int status;
        //wait(&status);

        printf("written string : '%s'\n", read_buf);
        exit(0);
    }
}
