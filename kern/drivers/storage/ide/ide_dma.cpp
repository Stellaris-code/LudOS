/*
ide_dma.cpp

Copyright (c) 23 Yann BOUCHER (yann)

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

#include "ide_dma.hpp"

#include "mem/memmap.hpp"
#include "io.hpp"
#include "utils/bitops.hpp"
#include "utils/memutils.hpp"
#include "time/timer.hpp"

#include "ide_common.hpp"

#include "utils/nop.hpp"

namespace ide::dma
{

struct PRD
{
    uint32_t phys_buf_addr;
    uint16_t byte_count;
    uint16_t reserved : 15;
    uint16_t end_of_prdt : 1;
};
static_assert(sizeof(PRD) == 8);

alignas(0x10000) static PRD PRDT[0x10000 / sizeof(PRD)];

volatile bool primary_master_int { false };
volatile bool primary_slave_int { false };
volatile bool secondary_master_int { false };
volatile bool secondary_slave_int { false };

volatile bool raised_ints[2][2] { {false, false}, {false, false} };
DiskException::ErrorType drive_status[2][2];

bool Controller::accept(const pci::PciDevice &dev)
{
    return dev.classCode == 0x1 && dev.subclass == 0x1;
}

void Controller::init()
{
    enable_bus_mastering();

    for (auto pair : scan())
    {
        ide::dma::Disk::create_disk(*this, (BusPort)pair.first, (DriveType)pair.second);
    }

    isr::register_handler(IRQ14, [this](const registers* r){return int14_handler(r);});
    isr::register_handler(IRQ15, [this](const registers* r){return int15_handler(r);});
}

bool Controller::common_handler(BusPort port)
{
    auto status = status_byte(port);

    if (status_byte(port) & (1<<2))
    {
        bool slave = drive_register(port) & (1<<4);

        raised_ints[port==BusPort::Primary][slave] = true;

        send_command_byte(port, 0); // clear start/stop bit

        if (status & (1<<1))
        {
            drive_status[port==BusPort::Primary][slave] = get_error(port);
        }
        else
        {
            drive_status[port==BusPort::Primary][slave] = DiskException::OK;
        }

        bit_clear(status, 0); bit_clear(status, 1); // clear error and interrupt bits
        send_status_byte(port, status);
    }

    return true;
}

bool Controller::int14_handler(const registers *regs)
{
    return common_handler(BusPort::Primary);
}

bool Controller::int15_handler(const registers *regs)
{
    return common_handler(BusPort::Secondary);
}

std::vector<std::pair<uint16_t, uint8_t> > Controller::scan()
{
    std::vector<std::pair<uint16_t, uint8_t>> result;

    for (auto bus : {Primary, Secondary, Third, Fourth})
    {
        for (auto type : {Master, Slave})
        {
            if (identify(io_base(bus), type))
            {
                result.emplace_back(bus, type);
            }
        }
    }
    return result;
}

void Controller::send_command(BusPort bus, DriveType type, uint8_t command, bool read, size_t block, size_t count, gsl::span<const uint8_t> data)
{
    auto status = status_byte(bus);
    bit_clear(status, 0); bit_clear(status, 1); // clear error and interrupt bits
    send_status_byte(bus, status);

    send_command_byte(bus, (!(read)&1) << 3); // set operation direction

    prepare_prdt(bus, data);

    select(io_base(bus), type, block, count);

    outb(io_base(bus) + 7, command);

    send_command_byte(bus, ((read&1) << 3) | 0b1); // set start bit
}

uint16_t Controller::io_base(BusPort bus)
{
    if (bus == BusPort::Primary)
    {
        auto port = pci::get_bar_val(m_dev, 0);
        if (port == 0 || port == 1) port = 0x1F0;

        return port;
    }
    else
    {
        auto port = pci::get_bar_val(m_dev, 2);
        if (port == 0 || port == 1) port = 0x170;

        return port;
    }
}

void Controller::prepare_prdt(BusPort bus, gsl::span<const uint8_t> data)
{
    PRD* prd_ptr = (PRD*)PRDT;

    uintptr_t begin = (uintptr_t)data.data();
    uintptr_t end = begin + data.size();

    //log_serial("From : 0x%x, to 0x%x\n", begin, end);

    for (size_t pg = begin; pg < end; pg += Memory::page_size())
    {
        size_t addr = Memory::physical_address((void*)pg);
        size_t size = Memory::page(pg) == Memory::page(end) ? Memory::offset(end) - Memory::offset(pg) : Memory::page_size() - Memory::offset(addr);

        //log_serial("Addr : 0x%x, Size : 0x%x\n", addr, size);

        pg = Memory::page(pg);

        prd_ptr->byte_count = size;
        prd_ptr->phys_buf_addr = addr;
        prd_ptr->end_of_prdt = (pg == Memory::page(end));

        ++prd_ptr;
    }

    send_prdt(bus);
}

uint8_t Controller::status_byte(BusPort bus)
{
    uint16_t port = pci::get_bar_val(m_dev, 4) + (bus==BusPort::Primary?0x2:0xA);

    return inb(port);
}

void Controller::send_status_byte(BusPort bus, uint8_t val)
{
    uint16_t port = pci::get_bar_val(m_dev, 4) + (bus==BusPort::Primary?0x2:0xA);

    outb(port, val);
}

void Controller::send_command_byte(BusPort bus, uint8_t val)
{
    uint16_t port = pci::get_bar_val(m_dev, 4) + (bus==BusPort::Primary?0x0:0x8);

    outb(port, val);
}

void Controller::send_prdt(BusPort bus)
{
    uint16_t port = pci::get_bar_val(m_dev, 4) + (bus==BusPort::Primary?0x4:0xC);

    outl(port, Memory::physical_address(PRDT));
}

Disk::Disk(Controller& controller, BusPort port, DriveType type)
    : IDEDisk(port, type), m_cont(controller)
{

}

// TODO : unify

MemBuffer Disk::read_sector(size_t sector, size_t count) const
{
    volatile auto& int_status = raised_ints[m_port==BusPort::Primary][m_type==DriveType::Slave];

    int_status = false;

    (void)status_register(m_port); // read status port to reset drive

    MemBuffer data(sector_size()*count);
    m_cont.send_command((BusPort)m_port, (DriveType)m_type, ata_read_dma_ex, true, sector, count, data);

    if (!Timer::sleep_until_int([&int_status]{return int_status;}, 2000))
    {
        throw DiskException(*this, DiskException::TimeOut);
    }

    int_status = false;

    auto status = drive_status[m_port==BusPort::Primary][m_type==DriveType::Slave];

    if (status != DiskException::OK)
    {
        throw DiskException(*this, status);
    }

    return data;
}

void Disk::write_sector(size_t sector, gsl::span<const uint8_t> data)
{
    volatile auto& int_status = raised_ints[m_port==BusPort::Primary][m_type==DriveType::Slave];

    int_status = false;

    (void)status_register(m_port); // read status port to reset drive

    size_t count = data.size() / sector_size() + (data.size()%sector_size()?1:0);

    m_cont.send_command((BusPort)m_port, (DriveType)m_type, ata_write_dma_ex, true, sector, count, data);

    if (!Timer::sleep_until_int([&int_status]{return int_status;}, 2000))
    {
        throw DiskException(*this, DiskException::TimeOut);
    }

    int_status = false;

    auto status = drive_status[m_port==BusPort::Primary][m_type==DriveType::Slave];

    if (status != DiskException::OK)
    {
        throw DiskException(*this, status);
    }
}

ADD_PCI_DRIVER(Controller);

}
