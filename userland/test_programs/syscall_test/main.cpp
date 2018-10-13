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

#include <string.hpp>
#include <liballoc/liballoc.h>
#include <sys/fnctl.h>
#include <errno.h>

#include "utils/stlutils.hpp"

void __attribute__((constructor)) init()
{
    print_debug("bouh!\n");
}

void __attribute__((constructor)) init2()
{
    print_debug("bouh2!\n");
}

void __attribute__((destructor)) tini()
{
    print_debug("goodbye!\n");
}

int main(int argc, char* argv[])
{
    //std::string std_str = "Bonjour depuis std::string !\n";

    printf("Epoch : %zd\n", time(nullptr));

    char local_str[] = "Arguments :\n";

    std::string std_str = "From std::string;\n";

    char* allocated_str = (char*)malloc(60);

    strcpy(allocated_str, "Bonjour depuis malloc!\n");

    print_debug("Am here\n");

    printf("arg count : %d\n", argc);
    for (size_t i { 0 }; i < argc; ++i)
    {
        printf("'%s'", argv[i]);
        printf("%s", "\n");
    }

    printf("From printf : %d %s accent : ééé\n", 66, "bonjour !");

    print_debug(allocated_str);
    print_debug(std_str.c_str());

//    print_serial(local_str);

    std::string process_info;
    char buf[512];
    getcwd(buf, 512);
    process_info += "Pwd : " + std::string(trim_zstr(buf).c_str());
    process_info += ", PID : " + std::to_string(getpid());

    printf("%s\n", process_info.c_str());

    const char path[] = "/home/test.txt";
    int fd = open(path, O_RDONLY, 0);
    printf("Opening '%s', return : %d, error : %s\n", path, fd, strerror(errno));
    std::vector<uint8_t> data(40);
    if (read(fd, data.data(), data.size()) == -1)
    {
        printf("Error reading %s : %s\n", path, strerror(errno));
    }
    else
    {
        data.emplace_back('\0');
        printf("Data : %s\n", data.data());
    }

    int proc_name_fd = open("/proc/self/name", O_RDONLY, 0);
    if (proc_name_fd < 0)
    {
        perror("proc self open"); return -1;
    }
    char name_buf[60];
    read(proc_name_fd, name_buf, sizeof(name_buf));
    printf("Process name : %s\n", name_buf);

    free(allocated_str);

    return 0;
}
