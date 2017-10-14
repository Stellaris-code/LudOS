/*
diskinterface.hpp

Copyright (c) 16 Yann BOUCHER (yann)

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
#ifndef DISKINTERFACE_HPP
#define DISKINTERFACE_HPP

#include <stdint.h>

#include <functional.hpp>
#include <vector.hpp>

class DiskInterface
{
public:
    enum class Error
    {
        OK,
        Unknown
    };

public:

    using ReadFunction  = std::function<bool(uint32_t sector, uint8_t count, uint8_t* buf)>;
    using WriteFunction = std::function<bool(uint32_t sector, uint8_t count, const uint8_t* buf)>;

    static inline bool read(size_t disk_num, uint32_t sector, uint8_t count, uint8_t* buf)
    {
        assert(disk_num < drive_count());
        return m_read_funs[disk_num](sector, count, buf);
    }

    static inline bool write(size_t disk_num, uint32_t sector, uint8_t count, const uint8_t* buf)
    {
        assert(disk_num < drive_count());
        return m_write_funs[disk_num](sector, count, buf);
    }

    static inline size_t drive_count()
    {
        return m_drive_count;
    }

    static inline size_t add_drive(ReadFunction read_fun, WriteFunction write_fun)
    {
        m_read_funs.emplace_back(read_fun);
        m_write_funs.emplace_back(write_fun);

        return ++m_drive_count;
    }

    static inline Error last_error { Error::OK };

private:
    static inline std::vector<ReadFunction> m_read_funs;
    static inline std::vector<WriteFunction> m_write_funs;

    static inline size_t m_drive_count { 0 };
};

#endif // DISKINTERFACE_HPP
