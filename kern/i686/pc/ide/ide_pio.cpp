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

bool ide::pio::read(BusPort port, ide::DriveType type, uint32_t block, uint8_t count, uint8_t *buf)
{
    detail::common(port, type, block, count);
    outb(port + 7, 0x20);

    for (size_t sector { 0 }; sector < count; ++sector)
    {
        detail::poll_bsy(port);
        detail::poll(port); // TODO : use IRQ !

        for (size_t i { 0 }; i < 256; ++i)
        {
            uint32_t tmpword = inw(port + 0);
            buf[sector*512+i * 2] = tmpword & 0xFF;
            buf[sector*512+i * 2 + 1] = (tmpword >> 8) & 0xFF;

            if (detail::error_set(port)) // reads status register and resets INTRQ
            {
                bool stat = !detail::error_set(port);
                if (!stat)
                {
                    detail::clear_error(port);

                    return stat;
                }
            }
        }
    }

    return true;
}


bool ide::pio::write(BusPort port, ide::DriveType type, uint32_t block, uint8_t count, const uint8_t *buf)
{
    detail::common(port, type, block, count);

    outb(port + 7, 0x30);

    detail::poll_bsy(port);
    detail::poll(port);

    for (int i = count * 256; --i >= 0;)
    {
        outw(port + 0, *buf++);

        detail::poll_bsy(port);
        detail::poll(port);

        if (detail::error_set(port))
        {
            bool stat = !detail::error_set(port);
            if (!stat)
            {
                detail::clear_error(port);

                return stat;
            }
        }
    }

    outb(port + 7, 0xE7); // cache flush

    return true;
}


void ide::pio::detail::poll(BusPort port)
{
    size_t max_iters { 0x10000 };
    while (!(inb(port + 7) & 0x08) && max_iters-- > 0) { nop(); }
}

void ide::pio::detail::poll_bsy(BusPort port)
{
    size_t max_iters { 0x10000 };
    while ((inb(port + 7) & 0x80) && max_iters-- > 0) { nop(); }
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
    for (auto bus : {Primary, Secondary, Third, Fourth})
    {
        for (auto type : {Master, Slave})
        {
            if (identify(bus, type))
            {
                DiskInterface::add_drive([bus, type](uint32_t sector, uint8_t count, uint8_t* buf)
                {
                    auto status = read(bus, type, sector, count, buf);
                    if (!status)
                    {
                        DiskInterface::last_error = DiskInterface::Error::Unknown;
                    }

                    return status;
                },
                [bus, type](uint32_t sector, uint8_t count, const uint8_t* buf)
                {
                    auto status = write(bus, type, sector, count, buf);
                    if (!status)
                    {
                        DiskInterface::last_error = DiskInterface::Error::Unknown;
                    }

                    return status;
                });
            }
        }
    }

    log("IDE PIO initialized.\n");
}

std::optional<ide::identify_data> ide::pio::identify(BusPort port, DriveType type)
{
    detail::common(port, type == Master ? 0xA0 : 0xB0, 0, 0);

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

        detail::poll(port);
        do
        {
            status = inb(port + 7);
            if(status & 0x01)
            {
                kprintf("ATA %s%s has ERR set. Disabled.\n", port_name, type==Master?" master":" slave");
                return {};
            }
        } while(!(status & 0x08));

        kprintf("ATA %s%s is online.\n", port_name, type==Master?" master":" slave");

        std::array<uint16_t, 256> buffer;

        detail::poll(port);

        for(size_t i = 0; i<256; i++)
        {
            buffer[i] = inw(port + 0);
        }

        identify_data* id_data;
        id_data = reinterpret_cast<identify_data*>(buffer.data());

        kprintf("Firmware : %s, model : %s\n", std::string(id_data->firmware,8).c_str(), std::string(id_data->model, 40).c_str());

        return *id_data;
    }
    else
    {
        kprintf("ATA %s%s doesn't exist.\n", port_name, type==Master?" master":" slave");

        return {};
    }
}

std::vector<std::pair<ide::BusPort, ide::DriveType>> ide::pio::scan()
{
    std::vector<std::pair<ide::BusPort, ide::DriveType>> result;

    for (auto bus : {Primary, Secondary, Third, Fourth})
    {
        for (auto type : {Master, Slave})
        {
            if (identify(bus, type))
            {
                result.emplace_back(bus, type);
            }
        }
    }
    return result;
}

