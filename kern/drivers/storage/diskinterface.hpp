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
#include <map.hpp>

#include "utils/memutils.hpp"
#include "utils/defs.hpp"
#include "panic.hpp"

class DiskInterface
{
public:
    enum class Error
    {
        OK,
        OutOfBounds,
        ReadOnly,
        Unknown
    };

    struct DiskInfo
    {
        size_t disk_size { 0 };
        size_t sector_size { 0 };
        std::string drive_name;
    };

public:

    using ReadFunction  = std::function<bool(size_t sector, size_t count, uint8_t* buf)>;
    using WriteFunction = std::function<bool(size_t sector, size_t count, const uint8_t* buf)>;
    using InfoFunction  = std::function<DiskInfo()>;

    [[nodiscard]]
    static inline bool read(size_t disk_num, size_t sector, size_t count, uint8_t* buf)
    {
        assert(disk_num < drive_count());

        return m_entries[disk_num].read(sector, count, buf);
    }

    [[nodiscard]]
    static inline bool write(size_t disk_num, size_t sector, size_t count, const uint8_t* buf)
    {
        assert(disk_num < drive_count());
        auto result = m_entries[disk_num].write(sector, count, buf);

#if  defined(LUDOS_TESTING)
        // check if write succeeded
        std::vector<uint8_t> data(512 * count);
        read(disk_num, sector, count, data.data());
        for (size_t i { 0 }; i < data.size(); ++i)
        {
            if (buf[i] != data[i]) err("%d : 0x%x/0x%x\n", i, buf[i], data[i]);
        }
#endif

        return result;
    }

    static inline size_t drive_count()
    {
        return m_drive_count;
    }

    static inline size_t add_drive(ReadFunction read_fun, WriteFunction write_fun, InfoFunction info_fun)
    {
        size_t i { 0 };
        while (m_entries.find(i) != m_entries.end())
        {
            ++i;
        }

        m_entries[i] = Entry{read_fun, write_fun, info_fun};

        return m_drive_count++;
    }

    static inline bool remove_drive(size_t disk_num)
    {
        return m_entries.erase(disk_num);
    }

    static inline size_t add_memory_drive(void* address, size_t size)
    {
        return add_drive([address, size](uint32_t sector, size_t count, uint8_t* buf)
        {
            if (sector*512 + count > size)
            {
                last_error = Error::OutOfBounds;
                warn("Out of bounds disk access\n");
                return false;
            }
            memcpy(buf, reinterpret_cast<const uint8_t*>(address) + sector*512, count*512);
            return true;
        },
        [address, size](uint32_t sector, size_t count, const uint8_t* buf)
        {
            if (sector*512 + count > size)
            {
                last_error = Error::OutOfBounds;
                warn("Out of bounds disk access\n");
                return false;
            }
            memcpy(reinterpret_cast<uint8_t*>(address) + sector*512, buf, count*512);
            return true;
        }, [address, size]
        {
            char buf[64];
            ksnprintf(buf, 64, "<memory %p:%p>", address, reinterpret_cast<uint8_t*>(address) + size);
            return DiskInfo{.disk_size = size,
                        .sector_size = 512,
                        buf};
        });
    }

    static inline size_t add_memory_drive(const void* address, size_t size)
    {
        return add_drive([address, size](uint32_t sector, size_t count, uint8_t* buf)
        {
            if (sector*512 + count > size)
            {
                last_error = Error::OutOfBounds;
                warn("Out of bounds disk access\n");
                return false;
            }
            memcpy(buf, reinterpret_cast<const uint8_t*>(address) + sector*512, count*512);
            return true;
        },
        [](uint32_t, size_t, const uint8_t*)
        {
            last_error = Error::ReadOnly;
            return false;
        }, [address, size]
        {
        char buf[64];
        ksnprintf(buf, 64, "<memory %p:%p>", address, reinterpret_cast<const uint8_t*>(address) + size);
        return DiskInfo{.disk_size = size,
                    .sector_size = 512,
                    buf};
    });
}

static inline DiskInfo info(size_t disk_num)
{
    assert(disk_num < drive_count());

    return m_entries[disk_num].info();
}

static inline Error last_error { Error::OK };

private:
struct Entry
{
    ReadFunction read;
    WriteFunction write;
    InfoFunction info;
};

private:
static inline std::map<size_t, Entry> m_entries;

static inline size_t m_drive_count { 0 };
};

#endif // DISKINTERFACE_HPP
