/*
main.cpp

Copyright (c) 21 Yann BOUCHER (yann)

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

#include <string.hpp>
#include <stdio.h>

#include <sys/fnctl.h>
#include <sys/fs.h>
#include <syscalls/syscall_list.hpp>

std::string read_str()
{
    std::string str;
    char c;
    while ((c = getchar()) != '\n')
    {
        str += c;
    }

    return str;
}

int get_int()
{
    auto str = read_str();
    return std::stoi(str);
}

uint64_t total_ticks()
{
    uint64_t ret;
    asm volatile ( "rdtsc" : "=A"(ret) );
    return ret;
}

int main(int argc, char* argv[])
{
    int number = 783; // TODO
    int tries = 0;

    while (true)
    {
        printf("Enter your guess : \n");
        int value = get_int();
        ++tries;
        if (value > number) printf("Target is lower!\n");
        if (value < number) printf("Target is higher!\n");
        if (value == number)
        {
            printf("You found it ! Great job !\n");
            exit(tries);
            break;
        }

//        int fd = open("/initrd/init.sh", O_RDONLY, 0);
//        char buf[52] = { 0 };
//        read(fd, buf, 20);
//        printf("Data : '%s'\n", buf);

//        lseek(fd, 10, SEEK_SET);
//        read(fd, buf, 20);
//        printf("Data : '%s'\n", buf);
    }
}
