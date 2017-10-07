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

#include "drivers/diskinterface.hpp"
#include "io.hpp"
#include "utils/bitops.hpp"
#include "utils/nop.hpp"

#include <array.hpp>
#include <string.hpp>

void ide::pio::detail::common(BusPort port, uint8_t type, uint32_t block, uint8_t count)
{
    outb(port + 6, type | ((block >> 24) & 0x0F));
    outb(port + 2, count);

    outb(port + 3, block & 0xFF);
    outb(port + 4, (block >> 8) & 0xFF);
    outb(port + 5, (block >> 16) & 0xFF);

    // 400ns
    for (size_t i { 0 }; i < 5; ++i)
    {
        inb(port + 6);
    }
}

bool ide::pio::read(BusPort port, ide::pio::DriveType type, uint32_t block, uint8_t count, uint8_t *buf)
{
    detail::common(port, type, block, count);
    outb(port + 7, 0x20);

    for (size_t sector { 0 }; sector < count; ++sector)
    {
        detail::poll(port); // TODO : use IRQ !
        for (size_t i { 0 }; i < 256; ++i)
        {
            uint32_t tmpword = inw(port + 0);
            buf[sector*512+i * 2] = tmpword & 0xFF;
            buf[sector*512+i * 2 + 1] = (tmpword >> 8) & 0xFF;
        }
    }

    bool stat = !detail::error_set(port);
    if (!stat)
    {
        detail::clear_error(port);
    }

    return stat;
}


bool ide::pio::write(BusPort port, ide::pio::DriveType type, uint32_t block, uint8_t count, const uint8_t *buf)
{
    detail::common(port, type, block, count);

    outb(port + 7, 0x30);

    for (size_t sector { 1 }; sector <= count; ++sector)
    {
        for (size_t i { 0 }; i < 256; ++i)
        {
            uint16_t tmpword = (buf[i*sector * 2 + 1] << 8) | buf[i*sector * 2];
            outw(port + 0, tmpword);
            if (detail::error_set(port))
            {
                err("Error during ide pio write : err : 0x%x, stat : 0x%x\n", error_register(port), status_register(port));
                return false;
            }
            for (size_t k { 0 }; k < 3; ++k)
            {
                nop();
            }
        }
    }

    bool stat = !detail::error_set(port);
    if (!stat)
    {
        detail::clear_error(port);
    }

    return stat;
}


void ide::pio::detail::poll(BusPort port)
{
    size_t max_iters { 0x10000 };
    while (!(inb(port + 7) & 0x08) && max_iters-- > 0) { nop(); }
}

void ide::pio::detail::flush(BusPort port)
{
    outb(port + 7, 0xE7);
}

bool ide::pio::detail::error_set(BusPort port)
{
    return bit_check(status_register(port), 0) || bit_check(status_register(port), 5);
}

void ide::pio::detail::clear_error(BusPort port)
{
    outb(port + 7, 0x00); // nop
}

uint8_t ide::pio::error_register(BusPort port)
{
    return inb(port + 1);
}

uint8_t ide::pio::status_register(BusPort port)
{
    return inb(port + 7);
}

void ide::pio::init()
{
    DiskInterface::read_impl = [](size_t disk_num, uint32_t sector, uint8_t count, uint8_t* buf)
    {
        BusPort port;
        if (disk_num/2 == 0)
        {
            port = Primary;
        }
        else if (disk_num/2 == 1)
        {
            port = Secondary;
        }
        else if (disk_num/2 == 2)
        {
            port = Third;
        }
        else
        {
            port = Fourth;
        }
        auto status = read(port, disk_num&0x1 ? Slave : Master, sector, count, buf);
        if (!status)
        {
            DiskInterface::last_error = DiskInterface::Error::Unknown;
        }

        return status;
    };

    DiskInterface::write_impl = [](size_t disk_num, uint32_t sector, uint8_t count, const uint8_t* buf)
    {
        BusPort port;
        if (disk_num/2 == 0)
        {
            port = Primary;
        }
        else if (disk_num/2 == 1)
        {
            port = Secondary;
        }
        else if (disk_num/2 == 2)
        {
            port = Third;
        }
        else
        {
            port = Fourth;
        }
        auto status = write(port, disk_num&0x1 ? Slave : Master, sector, count, buf);
        if (!status)
        {
            DiskInterface::last_error = DiskInterface::Error::Unknown;
        }

        return status;
    };

    DiskInterface::scan_impl = []
    {
        auto results = ide::pio::scan();
        std::vector<uint32_t> drives;

        for (const auto& result : results)
        {
            uint32_t drive;
            if (result.first == Primary)
            {
                drive = 0;
            }
            else if (result.first == Secondary)
            {
                drive = 2;
            }
            else if (result.first == Third)
            {
                drive = 4;
            }
            else
            {
                drive = 6;
            }

            if (result.second == Slave)
            {
                drive += 1;
            }

            drives.emplace_back(drive);
        }

        return drives;
    };

    log("IDE PIO initialized.\n");
}

bool ide::pio::detail::identify(ide::pio::BusPort port, ide::pio::DriveType type)
{
    common(port, type == Master ? 0xA0 : 0xB0, 0, 0);

    outb(port + 7, 0xEC);

    const char* port_name;
    if (port == Primary)
    {
        port_name = "Primary";
    }
    else if (port == Secondary)
    {
        port_name = "Secondary";
    }
    else if (port == Third)
    {
        port_name = "Third";
    }
    else
    {
        port_name = "Fourth";
    }

    uint8_t status = inb(port + 7);
    if (status)
    {

        poll(port);
        do
        {
            status = inb(port + 7);
            if(status & 0x01)
            {
                kprintf("ATA %s%s has ERR set. Disabled.\n", port_name, type==Master?" master":" slave");
                return false;
            }
        } while(!(status & 0x08));

        kprintf("ATA %s%s is online.\n", port_name, type==Master?" master":" slave");

        std::array<uint16_t, 256> buffer;

        poll(port);

        for(size_t i = 0; i<256; i++)
        {
            buffer[i] = inw(port + 0);
        }

        identify_data* id_data;
        id_data = reinterpret_cast<identify_data*>(buffer.data());

        kprintf("Firmware : %s, model : %s\n", std::string(id_data->firmware,8).c_str(), std::string(id_data->model, 40).c_str());

        return true;
    }
    else
    {
        kprintf("ATA %s%s doesn't exist.\n", port_name, type==Master?" master":" slave");

        return false;
    }
}

std::vector<std::pair<ide::pio::BusPort, ide::pio::DriveType>> ide::pio::scan()
{
    std::vector<std::pair<ide::pio::BusPort, ide::pio::DriveType>> result;

    for (auto bus : {Primary, Secondary, Third, Fourth})
    {
        for (auto type : {Master, Slave})
        {
            if (detail::identify(bus, type))
            {
                result.emplace_back(bus, type);
            }
        }
    }
    return result;
}
