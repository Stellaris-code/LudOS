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
#include <string.hpp>
#include <stdlib.h>
#include <sys/fnctl.h>
#include <sys/fs.h>
#include <sys/wait.h>

#include "utils/stlutils.hpp"

void __attribute__((constructor)) init()
{
    printf("Should show up first\n");
}

void ensure(bool cond)
{
    if (!cond)
    {
        fprintf(stderr, "TEST FAILED :\n");
        while (true) {}
        exit(1);
    }
}

int execute_program(const char* path)
{
    int ret = fork();
    if (ret < 0)
    {
        perror("fork");
        return -1;
    }
    else if (ret == 0)
    {
        const char* argv[] = {0};
        const char* envp[] = {0};
        if (execve(path, argv, envp) == -1)
        {
            perror("execve");
        }
        return -2;
    }
    else
    {
        int status;
        printf("Parent with child PID %d\n", ret);
        if (waitpid(ret, &status, 0) < 0)
        {
            perror("waitpid");
        }
        return WEXITSTATUS(status);
    }
}

void fork_test()
{
    printf("Before fork : \n");

    char* str = (char*)malloc(80);
    strcpy(str, "bonjour root");

    int ret = fork();
    if (ret < 0)
    {
        printf("Error : %s\n", strerror(errno));
    }
    else if (ret == 0)
    {
        ensure(strcmp(str, "bonjour root") == 0);
        printf("Child first : %s\n", str);
        strcpy(str, "bonjour child");
        printf("Child second : %s\n", str);

        sched_yield();
        exit(63);
    }
    else
    {
        int status;

        ensure(strcmp(str, "bonjour root") == 0);
        printf("Parent first : %s\n", str);
        strcpy(str, "bonjour parent");
        printf("Parent second : %s\n", str);

        if (waitpid(ret, &status, 0) < 0)
        {
            perror("waitpid");
        }

        ensure(WEXITSTATUS(status) == 63 && WTERMSIG(status) == 0);
    }
}

uint64_t total_ticks()
{
    uint64_t ret;
    asm volatile ( "rdtsc" : "=A"(ret) );
    return ret;
}

int main(int argc, char* argv[])
{
    void* heap_alloc = malloc(2566525);
    std::string str = "hello from c++";

    char* allocated_str = (char*)malloc(60);

    strcpy(allocated_str, "Bonjour depuis malloc!\n");

    print_debug("Am here\n");

    printf("str : %s\n", str.c_str());

    for (int i { 0 }; i < argc; ++i)
    {
        printf("%s", argv[i]);
        printf("%s", "\n");
    }

    std::string process_info;
    char buf[512];
    getcwd(buf, 512);
    process_info += "Pwd : " + trim_zstr(buf);
    process_info += ", PID : " + std::to_string(getpid());

    printf("%s\n", process_info.c_str());

    const char path[] = "/initrd/test.txt";
    int fd = open(path, O_RDONLY, 0);
    printf("Opening '%s', return : %d, error : %s\n", path, fd, strerror(errno));
    std::vector<uint8_t> data(4);
    if (read(fd, data.data(), data.size()) == -1)
    {
        perror("read");
    }
    else
    {
        data.emplace_back('\0');
        ensure(std::string((char*)data.data()) == "data");

        lseek(fd, 5, SEEK_SET);
        data.resize(4);
        read(fd, data.data(), data.size());
        data.emplace_back('\0');
        ensure(std::string((char*)data.data()) == "2222");
    }

    auto res = open("/initrd/test_programs/Tests", O_RDONLY, 0);
    ensure(res > 0); close(res);
    res = open("/initrd/test_programs/Tests/", O_RDONLY, 0);
    ensure(res == -1 && errno == ENOTDIR);
    res = open("ShittyShell", O_RDONLY, 0);
    ensure (res > 0); close(res);
    res = open("ShittyShell/", O_RDONLY, 0);
    ensure(res == -1 && errno == ENOTDIR);

    fork_test();

    uint64_t total_test_ticks = 0;

    for (size_t i { 0 }; i < 100; ++i)
    {
        uint64_t begin = total_ticks();
        ensure(execute_program("/initrd/test_programs/SyscallTest") == 0);
        total_test_ticks += (total_ticks() - begin) / 100000;
    }

    for (size_t i { 0 }; i < 5; ++i)
    {
        ensure(execute_program("/initrd/test_programs/ShmTest") == 0);
    }

    printf("Mean ticks : %zd\n", total_test_ticks / 100);

    printf("Everyting seems ok\n");

    int ret = execute_program("/initrd/test_programs/MoreOrLess");
    printf("Return : %d (0x%x)\n", ret, ret);
    execute_program("/initrd/test_programs/Sleep");
}
