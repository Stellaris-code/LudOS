/*
traps.cpp

Copyright (c) 18 Yann BOUCHER (yann)

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

#include "cpu/traps.hpp"

#include "panic.hpp"

#include "tasking/process.hpp"
#include "siginfo.h"

namespace traps
{

void fp_error(const FPError& error)
{
    siginfo_t sig;
    sig.si_signo = SIGFPE;
    switch (error.type)
    {
        case FPError::IntDivByZero:
            sig.si_code = FPE_INTDIV;
    }
    sig.si_addr = (void*)error.address;

    Process::current().raise(Process::current().pid, SIGFPE, sig);

    panic("FP Error!\n");
}

void ill_error(const IllegalOpError& error)
{
    siginfo_t sig;
    sig.si_signo = SIGILL;
    switch (error.type)
    {
        case IllegalOpError::IllOpcode:
            sig.si_code = ILL_ILLOPC;
    }
    sig.si_addr = (void*)error.address;

    Process::current().raise(Process::current().pid, SIGILL, sig);

    panic("Illegal opcode Error!\n");
}


}
