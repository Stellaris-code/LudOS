/*
ahci_portconfig.cpp

Copyright (c) 08 Yann BOUCHER (yann)

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

#include "ahci.hpp"

#include "time/timer.hpp"
#include "utils/logging.hpp"

namespace ahci
{

detail::PortType detail::get_port_type(size_t port_idx)
{
    auto& port = mem->ports[port_idx];

    uint32_t ssts = port.ssts;

    auto ipm = static_cast<HBAPortIpm>((ssts >> 8) & 0x0F);
    auto det = static_cast<HBAPortDet>(ssts & 0x0F);

    if (det != HBAPortDet::DevicePresent)	// Check drive status
        return PortType::Null;
    if (ipm != HBAPortIpm::Active)
        return PortType::Null;

    switch (static_cast<SATASig>(port.sig))
    {
        case SATASig::SATAPI:
            return PortType::SATAPI;
        case SATASig::SEMB:
            return PortType::SEMB;
        case SATASig::PM:
            return PortType::PM;
        default:
            return PortType::SATA;
    }
}

void detail::init_port(size_t port)
{
    if (mem->ports[port].cmd & (pxcmd_cr | pxcmd_st | pxcmd_fr | pxcmd_fre))
    {
        stop_port(port);
    }

    init_memory(port);

    clear_errs(port);

    init_port_interrupts(port);

    start_port(port);
}

void detail::reset_port(size_t port)
{
    log(Debug, "Resetting AHCI port %d\n", port);

    mem->ports[port].cmd &= ~pxcmd_st; // clear st bit

    if (!Timer::sleep_until([&]{return (mem->ports[port].cmd & pxcmd_cr) == 0;}, 500))
    {
        // Continue
    }

    mem->ports[port].sctl |= sctl_det; // Issue reset
    Timer::sleep(10);
    mem->ports[port].sctl &= ~sctl_det; // Issue reset

    //Timer::sleep_until([&]{return (mem->ports[port].ssts & ssts_det) == 1;});

    clear_errs(port);
}

void detail::clear_errs(size_t port)
{
    mem->ports[port].serr = 0xFFFFFFFF; // clear any error sets
}

void detail::init_memory(size_t port)
{
    memset(&cmdlists[port], 0, sizeof(cmdlists[port]));
    mem->ports[port].clb = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&cmdlists[port]));

    if (mem->s64a)
    {
        mem->ports[port].clbu = 0;
    }


    memset(&rcvfis[port], 0, sizeof(rcvfis[port]));
    mem->ports[port].fb = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&rcvfis[port]));

    if (mem->s64a)
    {
        mem->ports[port].fbu = 0;
    }

    memset(&cmdtables[port], 0, sizeof(cmdtables[port]));

    for (size_t i { 0 }; i < mem->ncs; ++i)
    {
        cmdlists[port].hdrs[i].prdtl = 8;
        cmdlists[port].hdrs[i].ctba = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&cmdtables[port]));
        if (mem->s64a)
        {
            cmdlists[port].hdrs[i].ctbau = 0;
        }
    }
}

void detail::init_port_interrupts(size_t port)
{
    mem->ports[port].ie |= int_dhr_setup;
    mem->ports[port].ie |= int_dma_setup;
}

int detail::free_slot(size_t port)
{
    if (get_port_type(port) == PortType::Null)
    {
        return -1;
    }
    if (!(mem->pi & (1<<port)))
    {
        return -1;
    }

    // If not set in SACT and CI, the slot is free
    uint32_t slots = (mem->ports[port].sact | mem->ports[port].ci);

    for (int i = 0; i < mem->ncs; i++)
    {
        if ((slots&1) == 0)
        {
            return i;
        }
        slots >>= 1;
    }

    return -1;
}


void detail::stop_port(size_t port)
{
    mem->ports[port].cmd &= ~pxcmd_st; // clear st bit

    if (!Timer::sleep_until([&]{return (mem->ports[port].cmd & pxcmd_cr) == 0;}, 500))
    {
        reset_port(port);
        return;
    }


    mem->ports[port].cmd &= ~pxcmd_fre; // clear fre bit

    if (!Timer::sleep_until([&]{return (mem->ports[port].cmd & pxcmd_fr) == 0;}, 500))
    {
        reset_port(port);
        return;
    }
}

void detail::start_port(size_t port)
{
    if (!Timer::sleep_until([&]{return (mem->ports[port].cmd & pxcmd_cr) == 0;}, 500))
    {
        reset_port(port);
        return;
    }

    mem->ports[port].cmd |= pxcmd_st;
    mem->ports[port].cmd |= pxcmd_fre;
}

bool detail::check_errors(size_t port)
{
    if (mem->ports[port].is & pxis_tfes)
    {
        if (mem->ports[port].is & pxis_hbfs)
        {
            warn("AHCI software error port %d\n", port);
        }

        mem->ports[port].cmd &= ~pxcmd_st;
        mem->ports[port].serr &= 0xFFFF0000;

        if (!Timer::sleep_until([&]{return (mem->ports[port].cmd & pxcmd_cr) == 0;}, 500))
        {
            reset_port(port);
            return true;
        }

        mem->ports[port].cmd |= pxcmd_st;

        if (!((mem->ports[port].is & pxis_ifns) || (mem->ports[port].is & pxis_ofs)))
        {
            // Fatal error, reset the port
            reset_port(port);
        }

        return true;
    }
    else
    {
        return false;
    }
}

}
