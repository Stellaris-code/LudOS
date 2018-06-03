/*
rtl8139.cpp

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

#include "rtl8139.hpp"

#include "drivers/pci/pci.hpp"

#include "io.hpp"

#include "time/timer.hpp"

#include "mem/memmap.hpp"

#include <limits.hpp>

enum Regs
{
    MAC = 0x0,
    MAR = 0x8,
    RBSTART = 0x30,
    CMD = 0x37,
    IMR = 0x3C,
    ISR = 0x3E,
    RCR = 0x44,
    CONFIG_1 = 0x52
};

void RTL8139::init()
{
    m_iobase = pci::get_bar_val(m_dev, 0);

    enable_bus_mastering();
    power_on();
    soft_reset();
    set_rcv_buf(m_rcv_buf.data());

    outb(m_iobase + CMD, 0x5); // enable Tx/Rx
    outl(m_iobase + RCR, 0xF); // Tell the nic to accept all valid packages

    NetworkDriver::add_nic(*this);
}

kpp::array<uint8_t, 6> RTL8139::mac_address() const
{
    kpp::array<uint8_t, 6> mac;

    for (size_t i { 0 }; i < 6; ++i)
    {
        mac[i] = inb(m_iobase + MAC + i);
    }

    return mac;
}

bool RTL8139::accept(const pci::PciDevice &dev)
{
    return dev.vendorID == 0x10ec && dev.deviceID == 0x8139;
}

void RTL8139::power_on()
{
    outb(m_iobase + CONFIG_1, 0);
}

void RTL8139::soft_reset()
{
    outb(m_iobase + CMD, 0x10);
    Timer::sleep_until([this]{ return (inb(m_iobase + CMD) & 0x10) == 0; }, 500);
}

void RTL8139::set_rcv_buf(uint8_t *ptr)
{
    outl(m_iobase + RBSTART, Memory::physical_address(ptr));
}

ADD_PCI_DRIVER(RTL8139)
