/*
rtl8139.hpp

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
#ifndef RTL8139_HPP
#define RTL8139_HPP

#include "drivers/pci/pcidriver.hpp"
#include "drivers/network/driver.hpp"

class RTL8139 : public NetworkDriver, public PciDriver
{
public:
    virtual void init() override;

    virtual std::array<uint8_t, 6> mac_address() const override;

    static bool accept(const pci::PciDevice& dev);

    virtual std::string name() const override { return "Realtek RTL8139"; }

private:
    void enable_bus_mastering();
    void power_on();
    void soft_reset();
    void set_rcv_buf(uint8_t* ptr);

private:
    uint16_t m_iobase {};
    std::array<uint8_t, 8192 + 16 + 1500> m_rcv_buf;
};

#endif // RTL8139_HPP
