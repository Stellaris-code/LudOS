/*
process_loader.hpp

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
#ifndef PROCESS_LOADER_HPP
#define PROCESS_LOADER_HPP

#include <optional.hpp>
#include <vector.hpp>

#include "utils/gsl/gsl_span.hpp"
#include "utils/logging.hpp"

class Process;

class ProcessLoader
{
public:
    static bool accept(gsl::span<const uint8_t> file);

    static std::unique_ptr<ProcessLoader> get(gsl::span<const uint8_t> file);

public:
    virtual std::unique_ptr<Process> load() = 0;
    virtual std::string file_type() const = 0;

public:
    void set_file(gsl::span<const uint8_t> file);

protected:
    gsl::span<const uint8_t> m_file;
};

namespace detail
{

using LoaderEntry = ProcessLoader*(*)(gsl::span<const uint8_t>);

constexpr size_t max_loaders { 0x2000 };

extern LoaderEntry loaders[max_loaders];
extern LoaderEntry* loaders_list_ptr;

#define ADD_PROCESS_LOADER(name) \
__attribute__((constructor)) void _init_process_loader_##name() \
{ \
    static_assert(std::is_base_of_v<ProcessLoader, name>); \
    *detail::loaders_list_ptr++ = [] (gsl::span<const uint8_t> file)->ProcessLoader* { \
        if (name::accept(file)) \
        { \
            auto ldr = new name; \
            ldr->set_file(file); \
            return ldr; \
        } \
        return nullptr; \
    }; \
}

}

#endif // PROCESS_LOADER_HPP
