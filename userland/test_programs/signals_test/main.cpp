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

#include "i686/cpu/registers.hpp"

#define ensure(cond) ensure_impl(cond, #cond)

void ensure_impl(bool cond, const char* line)
{
    if (!cond)
    {
        fprintf(stderr, "TEST FAILED : %s\n", line);
    }
}

volatile sig_atomic_t sig_num = 0xdeadbeef;
volatile sig_atomic_t segfault_check = 0;
volatile sig_atomic_t intdiv_check = 0;
volatile sig_atomic_t illop_check = 0;
extern "C" int do_ludos_syscall(uint32_t no, uint32_t args, uint32_t* arg_table);

void handler(int sig)
{
    sig_num = sig;

    print_serial("Booooooh\n");
    printf("Signal is : %d\n", sig);
}

void segfault_handler(int, siginfo_t* siginfo, void* ucontext)
{
    registers* regs = (registers*)ucontext;

    print_serial("We just had a segfault !!!!\n");
    printf("At address : %p\n", siginfo->si_addr);
    printf("eax : 0x%x,   ebx : 0x%x\n", regs->eax, regs->ebx);
    segfault_check = 1;
}

void intdiv_handler(int, siginfo_t*, void* ucontext)
{
    print_debug("Intdiv !\n");
    registers* regs = (registers*)ucontext;
    regs->eip += 2; // TODO : !!

    intdiv_check = 1;
}
void illop_handler(int, siginfo_t*, void* ucontext)
{
    print_debug("Illop !\n");
    registers* regs = (registers*)ucontext;
    regs->eip += 1;

    illop_check = 1;
}

int main(int argc, char* argv[])
{
    signal(SIGFPE, (sighandler_t)intdiv_handler);
    signal(SIGILL, (sighandler_t)illop_handler);

    //asm volatile ("div %0"::"a"(0));
    //ensure(intdiv_check == 1);
    //asm volatile ("ud2");
    //ensure(illop_check == 1);

    errno = EOK;
    ensure(signal(SIGTERM, SIG_IGN) == SIG_ERR); ensure(errno == EINVAL);
    ensure(signal(SIGSTOP, SIG_IGN) == SIG_ERR); ensure(errno == EINVAL);
    ensure(signal(SIGUSR1, SIG_IGN) != SIG_ERR);

    struct sigaction action, old;
    action.sa_handler = SIG_IGN;
    old.sa_flags = 56;
    printf("%d\n", sigaction(SIGTERM, &action, &old));
    printf("%d\n", sigaction(SIGSTOP, &action, &old));
    printf("%d\n", sigaction(SIGUSR1, &action, &old));
    if (old.sa_flags == 0) printf("Should be flags 0 in old : %d\n", old.sa_flags);

    errno = EOK;
    ensure(signal(SIGUSR1, handler) != SIG_ERR);

    action.sa_sigaction = segfault_handler;
    ensure(sigaction(SIGSEGV, &action, nullptr) != -1);

    int esp;
    asm volatile ("mov %%esp, %0\n":"=m"(esp):);
    printf("Stack is : 0x%x\n", esp);

    ensure(kill(getpid(), SIGUSR1) == 0);
    ensure(sig_num == SIGUSR1);

    volatile uint32_t* ptr = (volatile uint32_t*)0xDEADBEEF;
    *ptr = 5; // NOLINT

    ensure(segfault_check == true);

    return 0;
}
