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

#include <sys/wait.h>

int exec_cmd(const char* cmd)
{
    int cmd_ret;

    auto ret = fork();
    if (ret == -1)
    {
        perror("fork");
        return -1;
    }
    else if (ret == 0) // Child
    {
        const char* argv[] = {0};
        const char* envp[] = {0};
        if (execve(cmd, argv, envp) == -1)
        {
            perror("execve");
            exit(22);
        }
    }
    else // Parent
    {
        if (waitpid(ret, &cmd_ret, 0) == -1)
        {
            perror("waitpid");
            return -2;
        }
        return WEXITSTATUS(cmd_ret);
    }
}

void read_str(char* buf, size_t len)
{
    char c;
    while ((c = getchar()) != '\n' && --len != 0)
    {
        *buf++ = c;
    }
    *buf = '\0';
}

int main()
{
    const char* prompt = "lame_shell>";

    while (true)
    {
        printf("%s", prompt);
        char cmd[64];
        read_str(cmd, 64);
        if (strcmp(cmd, "exit") == 0) exit(0);

        char prgm_path[128];
        strncpy(prgm_path, "/initrd/test_programs/", 23);
        strncat(prgm_path, cmd, 64);
        auto ret = exec_cmd(prgm_path);
        printf("Command '%s' returned code %d (0x%x)\n", cmd, ret, ret);
    }
}
