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

#include <syscalls/syscall_list.hpp>

#include <errno.h>
#include <stdio.h>

void ensure(bool cond)
{
    if (!cond)
    {
        fprintf(stderr, "TEST FAILED :\n");
        while (true) {}
        exit(1);
    }
}

int main(int argc, char* argv[])
{
    printf("%d\n", signal(SIGTERM, SIG_IGN)); perror("1");
    printf("%d\n", signal(SIGSTOP, SIG_IGN)); perror("1");
    printf("%d\n", signal(SIGUSR1, SIG_IGN)); perror("1");

    struct sigaction action, old;
    action.sa_handler = SIG_IGN;
    old.sa_flags = 56;
    printf("%d\n", sigaction(SIGTERM, &action, &old)); perror("1");
    printf("%d\n", sigaction(SIGSTOP, &action, &old)); perror("1");
    printf("%d\n", sigaction(SIGUSR1, &action, &old)); perror("1");
    if (old.sa_flags == 0) printf("Should be flags 0 in old : %d\n", old.sa_flags);

    while (true){}
    return 0;
}
