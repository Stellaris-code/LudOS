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
#include <string.h>
#include <stdio.h>

#include <sys/ipc.h>
#include <sys/wait.h>

char* shared_buf;

int main()
{
    int shm_id = shmget(IPC_PRIVATE, 1024, IPC_CREAT|IPC_EXCL);
    printf("shm_id : %d\n", shm_id);
    if (shm_id < 0)
    {
        perror("shmget");
        exit(1);
    }

    shared_buf = (char*)shmat(shm_id, 0, 0);
    if (shared_buf == (char*)-1)
    {
        perror("shmat");
        exit(1);
    }
    strcpy(shared_buf, "INITIAL");

    int ret = fork();
    if (ret == -1)
    {
        perror("fork");
        exit(1);
    }
    else if (ret == 0)
    { // Child
        printf("Child shows : %s\n", shared_buf);
        if (strcmp(shared_buf, "PARENT_MODIFIED") != 0)
        {
            exit(3);
        }

        strcpy(shared_buf, "CHILD_MODIFIED");
        printf("Child again shows : %s\n", shared_buf);
        if (strcmp(shared_buf, "CHILD_MODIFIED") != 0)
        {
            exit(4);
        }

        exit(0);
    }
    else
    { // Parent
        printf("Parent shows : %s\n", shared_buf);
        if (strcmp(shared_buf, "INITIAL") != 0)
        {
            exit(2);
        }

        strcpy(shared_buf, "PARENT_MODIFIED");
        printf("Parent shows now : %s\n", shared_buf);
        int status;
        waitpid(ret, &status, 0);

        printf("Parent shows after child : %s\n", shared_buf);
        if (strcmp(shared_buf, "CHILD_MODIFIED") != 0)
        {
            exit(4);
        }

        exit(WEXITSTATUS(status));
    }
}
