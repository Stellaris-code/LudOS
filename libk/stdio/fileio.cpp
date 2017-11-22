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

#include <stdio.h>
#include <errno.h>

#include "fs/vfs.hpp"

std::vector<uint8_t> fprintf_data;

FILE stdin_real { 0 };
FILE stdout_real { 1 };
FILE stderr_real { 2 };

FILE* stdin { &stdin_real };
FILE* stdout { &stdout_real };
FILE* stderr { &stderr_real };

int fprintf(FILE * stream, const char * format, ...)
{
    if (stream->fd >= vfs::descriptors.size())
    {
        errno = EBADFD;
        return -1;
    }

    auto& node = vfs::descriptors[stream->fd];

    va_list va;
    va_start(va, format);
    tfp_format(nullptr, [](void*, char c){ fprintf_data.emplace_back(c); }, format, va);
    va_end(va);

    size_t size = node.get().write(fprintf_data.data(), fprintf_data.size());
    if (!size)
    {
        errno = EIO;
        return -1;
    }

    fprintf_data.clear();

    return size;
}

FILE * fopen(const char * filename, const char * mode)
{
    // TODO : support modifiers

    auto point = vfs::find(filename);
    if (!point)
    {
        errno = ENOENT;
        return nullptr;
    }
    else
    {
        return new FILE {vfs::new_descriptor(*point)};
    }
}

int fclose( FILE * stream )
{
    // TODO : actually do something

    delete stream;
}
