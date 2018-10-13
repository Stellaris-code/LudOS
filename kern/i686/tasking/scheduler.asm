extern do_kernel_yield
extern kernel_yield_regs

global _ZN7tasking12kernel_yieldEv

_ZN7tasking12kernel_yieldEv: ; tasking::kernel_yield()

save_registers:
    mov [kernel_yield_regs+0x0], gs
    mov [kernel_yield_regs+0x4], fs
    mov [kernel_yield_regs+0x8], es
    mov [kernel_yield_regs+0xc], ds

    mov [kernel_yield_regs+0x10], edi ; restore gprs
    mov [kernel_yield_regs+0x14], esi
    mov [kernel_yield_regs+0x18], ebp
    mov [kernel_yield_regs+0x20], ebx
    mov [kernel_yield_regs+0x24], edx
    mov [kernel_yield_regs+0x28], ecx
    mov [kernel_yield_regs+0x2c], eax

    mov eax, [esp] ; return address
    mov [kernel_yield_regs+0x38], eax ; eip
    mov [kernel_yield_regs+0x3c], cs
    pushfd
        mov eax, [esp] ; eflags
        mov [kernel_yield_regs+0x40], eax
    lea esp, [esp+0x8] ; pushfd: 4; account for the return address: 4
    mov [kernel_yield_regs+0x44], esp
    mov [kernel_yield_regs+0x48], ss

    jmp do_kernel_yield
