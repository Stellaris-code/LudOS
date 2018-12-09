/*
page_fault_handler.cpp

Copyright (c) 03 Yann BOUCHER (yann)

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

#include "page_fault.hpp"

#include "tasking/process.hpp"
#include "siginfo.h"
#include "memmap.hpp"

#include <unordered_map.hpp>

#include "utils/stlutils.hpp"
#include "panic.hpp"

static std::unordered_map<uintptr_t, fault_callback> handlers;

fault_handle attach_fault_handler(void* v_addr, const fault_callback& handler)
{
    const auto page = Memory::page((uintptr_t)v_addr);

    assert(handlers.count(page) == 0);

    handlers[page] = handler;

    return page; // the page is the handle
}

void detach_fault_handler(fault_handle hdl)
{
    assert(handlers.count(hdl));

    handlers.erase(hdl);
}

void user_space_fault(const PageFault& fault)
{
    log_serial("User space fault for PID %d at 0x%x\n", Process::current().pid, fault.address);

    siginfo_t info;
    info.si_signo = SIGSEGV;
    info.si_code  = fault.type == PageFault::NonPresent ? SEGV_MAPERR : SEGV_ACCERR;
    info.si_addr = (void*)fault.address;

    Process::current().raise(Process::current().pid, SIGSEGV, info);
}

void kernel_page_fault(const PageFault& fault)
{
    kpp::string message;

    if (fault.type == PageFault::Write)
    {
        message += "write ";
    }
    else if (fault.type == PageFault::Execute)
    {
        message += "instruction fetch ";
    }
    else
    {
        message += "read ";
    }

    message += "at ";
    char buf[16];
    ksnprintf(buf, 16, "0x%x ", fault.address);
    message += trim_zstr(buf);

    if (fault.error == PageFault::Protection)
    {
        message += "(page exists)";
    }
    else
    {
        message += "(page doesn't exist)";
    }

    message[0] = toupper(message[0]); // Capitalize

    if (fault.address == 0)
    {
        panic("Null pointer access : %s\n", message.c_str());
    }
    else
    {
        panic("Page fault : %s\n", message.c_str());
    }
}

void page_fault_entry(const PageFault& fault)
{
    if (auto handler = handlers.find(Memory::page(fault.address)); handler != handlers.end())
    {
        if (handler->second(fault))
        {
            return; // the fault was succesfully handled by the handler
        }
    }

    if (fault.level == PageFault::Kernel)
    {
        kernel_page_fault(fault);
    }
    else
    {
        user_space_fault(fault);
    }
}
