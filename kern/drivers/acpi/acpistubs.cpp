/*
acpistubs.cpp

Copyright (c) 13 Yann BOUCHER (yann)

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

#include "acpi.h"

#include <stdarg.h>

#include "panic.hpp"
#include "utils/logging.hpp"
#include "mem/memmap.hpp"
#include "time/timer.hpp"
#include "time/time.hpp"
#include "drivers/pci/pci.hpp"
#include "i686/interrupts/isr.hpp"
#include "io.hpp"

#include <stdlib.h>

extern "C"
{
ACPI_STATUS AcpiOsInitialize()
{
    return AE_OK;
}

ACPI_STATUS AcpiOsTerminate()
{
    log(Notice, "ACPI shutdown\n");
    return AE_OK;
}

ACPI_PHYSICAL_ADDRESS AcpiOsGetRootPointer()
{
    ACPI_PHYSICAL_ADDRESS  Ret;
    Ret = 0;
    AcpiFindRootPointer(&Ret);
    return Ret;
}

ACPI_STATUS AcpiOsPredefinedOverride(const ACPI_PREDEFINED_NAMES *PredefinedObject, ACPI_STRING *NewValue)
{
    *NewValue = nullptr;

    return AE_OK;
}

ACPI_STATUS AcpiOsTableOverride(ACPI_TABLE_HEADER *ExistingTable, ACPI_TABLE_HEADER **NewTable)
{
    *NewTable = nullptr;

    return AE_OK;
}

void *AcpiOsMapMemory(ACPI_PHYSICAL_ADDRESS PhysicalAddress, ACPI_SIZE Length)
{
    return Memory::mmap(reinterpret_cast<void*>(PhysicalAddress), Length);
}

void AcpiOsUnmapMemory(void *LogicalAddress, ACPI_SIZE Length)
{
    //Memory::unmap(LogicalAddress, Length);
}

ACPI_STATUS AcpiOsGetPhysicalAddress(void *LogicalAddress, ACPI_PHYSICAL_ADDRESS *PhysicalAddress)
{
    *PhysicalAddress = Memory::physical_address(LogicalAddress);
    return AE_OK;
}

void *AcpiOsAllocate(ACPI_SIZE Size)
{
    return kmalloc(Size);
}

void AcpiOsFree(void *Memory)
{
    kfree(Memory);
}

// TODO : implement
BOOLEAN AcpiOsReadable(void *Memory, ACPI_SIZE Length)
{
    panic("");
    return true;
}

BOOLEAN AcpiOsWritable(void *Memory, ACPI_SIZE Length)
{
    panic("");
    return false;
}

ACPI_THREAD_ID AcpiOsGetThreadId()
{
    return 1; // TODO : implement
}

ACPI_STATUS AcpiOsExecute(ACPI_EXECUTE_TYPE Type, ACPI_OSD_EXEC_CALLBACK Function, void *Context)
{
    warn("AcpiOsExecute : Not implemented yet");

    return AE_NOT_IMPLEMENTED;
}

void AcpiOsSleep(UINT64 Milliseconds)
{
    warn("AcpiOsSleep : Not implemented yet");
}

void AcpiOsStall(UINT32 Microseconds)
{
    Timer::sleep(Microseconds*1000);
}

UINT64 AcpiOsGetTimer()
{
    return UINT64(Time::uptime()*1000ULL*1000ULL*10UL);
}

ACPI_STATUS AcpiOsSignal(UINT32 fun, void* info)
{
    if (fun == ACPI_SIGNAL_FATAL)
    {
        auto* fatal_info = (ACPI_SIGNAL_FATAL_INFO*)info;
        panic("Fatal ACPI signal ! Type : 0x%x, Code : 0x%x, Argument : 0x%x\n", fatal_info->Type, fatal_info->Code, fatal_info->Argument);
    }
    else if (fun == ACPI_SIGNAL_BREAKPOINT)
    {
        log(Notice, "ACPI Breakpoint : Message : '%s'\n", (const char*)info);
        asm ("xchgw %bx, %bx"); // bochs magic breakpoint

        return AE_OK;
    }
    else
    {
        return AE_OK; // ignore
    }
}

ACPI_STATUS
AcpiOsReadPciConfiguration (
        ACPI_PCI_ID             *PciId,
        UINT32                  Reg,
        UINT64                  *Value,
        UINT32                  Width)
{
    if (Width != 16)
    {
        warn("AcpiOsReadPciConfiguration : not implemented yet for Width %d\n", Width);
        return AE_NOT_IMPLEMENTED;
    }
    else
    {
        *Value = pci::read_reg(PciId->Bus, PciId->Device, PciId->Function, Reg);
        return AE_OK;
    }
}

ACPI_STATUS
AcpiOsWritePciConfiguration (
        ACPI_PCI_ID             *PciId,
        UINT32                  Reg,
        UINT64                  Value,
        UINT32                  Width)
{
    if (Width != 16)
    {
        warn("AcpiOsWritePciConfiguration : not implemented yet for Width %d\n", Width);
        return AE_NOT_IMPLEMENTED;
    }
    else
    {
        pci::write_reg(PciId->Bus, PciId->Device, PciId->Function, Reg, Value);
        return AE_OK;
    }

    return AE_NOT_IMPLEMENTED;
}

void ACPI_INTERNAL_VAR_XFACE AcpiOsPrintf(const char* fmt, ...)
{
    if (log_level >= Debug)
    {
        va_list va;
        va_start(va, fmt);
        tfp_format(nullptr, [](void*, char c){putchar(c);}, fmt, va);
        va_end(va);
    }
}

void ACPI_INTERNAL_VAR_XFACE AcpiOsVprintf(const char* fmt, va_list args)
{
    if (log_level >= Debug)
    {
        tfp_format(nullptr, [](void*, char c){putchar(c);}, fmt, args);
    }
}

ACPI_STATUS AcpiOsCreateSemaphore(UINT32 max, UINT32 initial, ACPI_SEMAPHORE* handle)
{
    return AE_OK;
}

ACPI_STATUS AcpiOsDeleteSemaphore(ACPI_SEMAPHORE handle)
{
    return AE_OK;
}

ACPI_STATUS AcpiOsWaitSemaphore(ACPI_SEMAPHORE handle, UINT32 units, UINT16 timeout)
{
    return AE_OK;
}

ACPI_STATUS AcpiOsSignalSemaphore(ACPI_SEMAPHORE handle, UINT32 units)
{
    return AE_OK;
}

ACPI_CPU_FLAGS AcpiOsAcquireLock(ACPI_SPINLOCK lock)
{
    //    LOCK_VAL(lock);

    //    return AE_OK;
    return AE_NOT_IMPLEMENTED;
}

void AcpiOsReleaseLock(ACPI_SPINLOCK lock, ACPI_CPU_FLAGS flags)
{
    //UNLOCK_VAL(lock);
}

ACPI_STATUS AcpiOsCreateLock(ACPI_SPINLOCK *lock)
{
    if (lock == nullptr)
    {
        return AE_BAD_PARAMETER;
    }

    *lock = new spinlock_t;

    if (*lock == nullptr)
    {
        return AE_NO_MEMORY;
    }

    return AE_OK;
}

void AcpiOsDeleteLock(ACPI_SPINLOCK lock)
{
    kfree(reinterpret_cast<void*>(const_cast<int*>(lock)));
}

ACPI_STATUS AcpiOsReadMemory(ACPI_PHYSICAL_ADDRESS addr, UINT64* value, UINT32 width)
{
    *value = *(uint64_t*)addr;

    return AE_OK;
}

ACPI_STATUS AcpiOsWriteMemory(ACPI_PHYSICAL_ADDRESS addr, UINT64 value, UINT32 width)
{
    *(uint64_t*)addr = value;

    return AE_OK;
}

ACPI_STATUS
AcpiOsEnterSleep (
        UINT8                   SleepState,
        UINT32                  RegaValue,
        UINT32                  RegbValue)
{
    return AE_OK;
}

ACPI_STATUS AcpiOsReadPort(ACPI_IO_ADDRESS addr, UINT32* value, UINT32 width)
{
    switch (width)
    {
        case 8:
            *value = inb(addr);
            return AE_OK;
        case 16:
            *value = inw(addr);
            return AE_OK;
        case 32:
            *value = inl(addr);
            return AE_OK;
    }

    return AE_ERROR;
}

ACPI_STATUS AcpiOsWritePort(ACPI_IO_ADDRESS addr, UINT32 value, UINT32 width)
{
    switch (width)
    {
        case 8:
            outb(addr, value);
            return AE_OK;
        case 16:
            outw(addr, value);
            return AE_OK;
        case 32:
            outl(addr, value);
            return AE_OK;
    }

    return AE_ERROR;
}

ACPI_STATUS AcpiOsPhysicalTableOverride(ACPI_TABLE_HEADER* exTable, ACPI_PHYSICAL_ADDRESS* addr, UINT32* length)
{
    *addr = 0;

    return AE_OK;
}

void AcpiOsWaitEventsComplete()
{
}

struct AcpicaIntHandler
{
    ACPI_OSD_HANDLER hand;
    void* context;
};

AcpicaIntHandler acpi_handlers[256];

ACPI_STATUS AcpiOsInstallInterruptHandler(UINT32 intLevel, ACPI_OSD_HANDLER hand, void* context)
{
    auto lambd = [](const registers* const regs)
    {
        acpi_handlers[regs->int_no].hand(acpi_handlers[regs->int_no].context);
        return true;
    };

    isr::register_handler(intLevel, lambd);

    return AE_OK;
}

ACPI_STATUS AcpiOsRemoveInterruptHandler(UINT32 int_no, ACPI_OSD_HANDLER hand)
{
    isr::delete_handler(int_no);

    return AE_OK;
}
}
