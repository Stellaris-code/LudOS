/*
fprintf.cpp

Copyright (c) 04 Yann BOUCHER (yann)

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

#ifdef LUDOS_USER

#include <stdio.h>
#include <errno.h>
#include <syscalls/syscall_list.hpp>
#include <sys/fnctl.h>

FILE stdin_real { 0 };
FILE stdout_real { 1 };
FILE stderr_real { 2 };

FILE* stdin { &stdin_real };
FILE* stdout { &stdout_real };
FILE* stderr { &stderr_real };

int fprintf(FILE * stream, const char * format, ...)
{
    char buf[1024];

    va_list va;
    va_start(va, format);
    size_t size = vsnprintf(buf, sizeof(buf), format, va);
    va_end(va);

    auto ret = write(stream->fd, buf, size);
    if (ret == -1)
    {
        return -1;
    }

    return size;
}

FILE * fopen(const char * filename, const char * mode)
{
    // TODO : support modifiers

    auto ret = open(filename, O_RDWR, 0);
    if (ret == -1)
    {
        return nullptr;
    }
    else
    {
        return new FILE { (size_t)ret };
    }
}

int fclose( FILE * stream )
{
    if (close(stream->fd) == -1)
    {
        return EOF;
    }

    delete stream;

    return 0;
}

int fflush( FILE * stream )
{
    // NOP

    return 0;
}

#endif
