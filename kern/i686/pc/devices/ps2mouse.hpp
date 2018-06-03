/*
ps2mouse.hpp

Copyright (c) 27 Yann BOUCHER (yann)

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
#ifndef PS2MOUSE_HPP
#define PS2MOUSE_HPP

#include "i686/cpu/registers.hpp"

#include "drivers/driver.hpp"

class PS2Mouse : public Driver
{
public:
    PS2Mouse();

public:
    static bool accept();

    void enable();
    void disable();

    void set_sample_rate(uint8_t rate);

    virtual kpp::string driver_name() const override;

private:
    bool isr(const registers* regs);

    void send_write(uint8_t val);
    uint8_t read();

    bool enable_intellimouse();

private:
    bool is_intellimouse { false };
};

#endif // PS2MOUSE_HPP
