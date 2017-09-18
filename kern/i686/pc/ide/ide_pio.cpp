/*
ide_pio.cpp

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

#include "ide_pio.hpp"

#include "utils/logging.hpp"

#include "diskinterface.hpp"
#include "io.hpp"
#include "utils/bitops.hpp"
#include "nop.hpp"

void ide::pio::detail::common(DriveType type, uint32_t block, uint8_t count)
{
    outb(0x1F6, type | ((block >> 24) & 0x0F));
    outb(0x1F2, count);

    outb(0x1F3, block & 0xFF);
    outb(0x1F4, (block >> 8) & 0xFF);
    outb(0x1F5, (block >> 16) & 0xFF);
}

bool ide::pio::read(ide::pio::DriveType type, uint32_t block, uint8_t count, uint8_t *buf)
{
    detail::common(type, block, count);

    outb(0x1F7, 0x20);
    for (size_t sector { 0 }; sector < count; ++sector)
    {
        detail::poll(); // TODO : use IRQ !
        for (size_t i { 0 }; i < 256; ++i)
        {
            uint32_t tmpword = inw(0x1F0);
            buf[sector*512+i * 2] = tmpword & 0xFF;
            buf[sector*512+i * 2 + 1] = (tmpword >> 8) & 0xFF;
        }
    }

    bool stat = !detail::error_set();
    if (!stat)
    {
        detail::clear_error();
    }

    return stat;
}


bool ide::pio::write(ide::pio::DriveType type, uint32_t block, uint8_t count, const uint8_t *buf)
{
    detail::common(type, block, count);

    outb(0x1F7, 0x30);

    for (size_t sector { 1 }; sector <= count; ++sector)
    {
        for (size_t i { 0 }; i < 256; ++i)
        {
            uint16_t tmpword = (buf[i*sector * 2 + 1] << 8) | buf[i*sector * 2];
            outw(0x1F0, tmpword);
            if (detail::error_set())
            {
                err("Error during ide pio write : err : 0x%x, stat : 0x%x\n", error_register(), status_register());
                return false;
            }
            for (size_t k { 0 }; k < 3; ++k)
            {
                nop();
            }
        }
    }

    bool stat = !detail::error_set();
    if (!stat)
    {
        detail::clear_error();
    }

    return stat;
}


void ide::pio::detail::poll()
{
    while (!(inb(0x1F7) & 0x08)) { nop(); }
}

void ide::pio::detail::flush()
{
    outb(0x1F7, 0xE7);
}

bool ide::pio::detail::error_set()
{
    return bit_check(status_register(), 0) || bit_check(status_register(), 5);
}

void ide::pio::detail::clear_error()
{
    outb(0x1F7, 0x00); // nop
}

uint8_t ide::pio::error_register()
{
    return inb(0x1F1);
}

uint8_t ide::pio::status_register()
{
    return inb(0x1F7);
}

void ide::pio::init()
{
    DiskInterface::read = [](size_t disk_num, uint32_t sector, uint8_t count, uint8_t* buf)
    {
        auto status = read(disk_num == 0 ? Master : Slave, sector, count, buf);
        if (!status)
        {
            DiskInterface::last_error = DiskInterface::Error::Unknown;
        }

        return status;
    };

    DiskInterface::write = [](size_t disk_num, uint32_t sector, uint8_t count, const uint8_t* buf)
    {
        auto status = write(disk_num == 0 ? Master : Slave, sector, count, buf);
        if (!status)
        {
            DiskInterface::last_error = DiskInterface::Error::Unknown;
        }

        return status;
    };

    log("IDE PIO initialized.\n");
}
