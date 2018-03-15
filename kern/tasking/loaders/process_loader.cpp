/*
process_loader.cpp

Copyright (c) 09 Yann BOUCHER (yann)

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

#include "process_loader.hpp"

#include "tasking/process.hpp"

namespace detail
{
LoaderEntry loaders[max_loaders];
LoaderEntry* loaders_list_ptr = loaders;
}

std::unique_ptr<ProcessLoader> ProcessLoader::get(gsl::span<const uint8_t> file)
{
    using namespace detail;
    for (LoaderEntry* ptr = loaders; ptr < loaders_list_ptr; ++ptr)
    {
        auto ldr = (*ptr)(file);
        if (ldr) return std::unique_ptr<ProcessLoader>(ldr);
    }

    return nullptr;
}

void ProcessLoader::set_file(gsl::span<const uint8_t> file)
{
    m_file = file;
}
