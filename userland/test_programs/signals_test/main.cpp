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
    }
}

sig_atomic_t sig_num = 0xdeadbeef;
extern "C" int do_ludos_syscall(uint32_t no, uint32_t args, uint32_t* arg_table);

void handler(int sig)
{
    sig_num = sig;

    print_serial("Booooooh\n");
    printf("Signal is : %d\n", sig);
}

int main(int argc, char* argv[])
{
    errno = EOK;
    ensure(signal(SIGTERM, SIG_IGN) == SIG_ERR);
    ensure(signal(SIGSTOP, SIG_IGN) == SIG_ERR);
    ensure(signal(SIGUSR1, SIG_IGN) == SIG_ERR); ensure(errno == EINVAL);

    struct sigaction action, old;
    action.sa_handler = SIG_IGN;
    old.sa_flags = 56;
    printf("%d\n", sigaction(SIGTERM, &action, &old)); perror("1");
    printf("%d\n", sigaction(SIGSTOP, &action, &old)); perror("1");
    printf("%d\n", sigaction(SIGUSR1, &action, &old)); perror("1");
    if (old.sa_flags == 0) printf("Should be flags 0 in old : %d\n", old.sa_flags);

    errno = EOK;
    ensure(signal(SIGUSR1, handler) != SIG_ERR); perror("1");

    int esp;
    asm volatile ("mov %%esp, %0\n":"=m"(esp):);
    printf("Stack is : 0x%x\n", esp);

    ensure(kill(0, SIGUSR1) == 0);

    printf("Signal received : %d\n", sig_num);
    print_serial("Seems okay");

    volatile uint32_t* ptr = (volatile uint32_t*)0xDEADBEEF;
    *ptr = 5; // NOLINT

    while (true){}
    return 0;
}
