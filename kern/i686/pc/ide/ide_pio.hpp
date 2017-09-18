/*
ide_pio.hpp

Copyright (c) 15 Yann BOUCHER (yann)

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
#ifndef IDE_PIO_HPP
#define IDE_PIO_HPP

#include <stdint.h>

namespace ide
{
namespace pio
{

    enum DriveType : uint8_t
    {
        Master = 0xE0,
        Slave = 0xF0
    };

    void init();

    bool read(DriveType type, uint32_t block, uint8_t count, uint8_t* buf);
    bool write(DriveType type, uint32_t block, uint8_t count, const uint8_t* buf);

    uint8_t error_register();
    uint8_t status_register();

    namespace detail
    {
        void common(DriveType type, uint32_t block, uint8_t count);
        void poll();
        void flush();
        bool error_set();
        void clear_error();
    }
}
}

#endif // IDE_PIO_HPP
