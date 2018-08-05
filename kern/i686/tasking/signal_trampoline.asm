global signal_trampoline
global signal_trampoline_info

%include "../kern/i686/defs.asm"

TRAMPOLINE_VIRTUAL_BASE equ KERNEL_VIRTUAL_BASE - 2*PAGE_SIZE

%define virtual_addr(sym) TRAMPOLINE_VIRTUAL_BASE+(sym-signal_trampoline)

align 0x1000

signal_trampoline:
    test dword [virtual_addr(signal_trampoline_info)+0x10], 0x4 ; test for SA_SIGINFO
    jnz siginfo_path

no_siginfo_path:
    sub esp, 8 ; 16B boundary
    push dword [virtual_addr(signal_trampoline_info)] ; the signal number
    call [virtual_addr(signal_trampoline_info)+0xc] ; the handler
    add esp, 12
    jmp exit

siginfo_path:
    push dword [virtual_addr(signal_trampoline_info)+0x8] ; ucontext*
    push dword [virtual_addr(signal_trampoline_info)+0x4] ; siginfo_t*
    push dword [virtual_addr(signal_trampoline_info)] ; the signal number
    call [virtual_addr(signal_trampoline_info)+0xc] ; the handler
    add esp, 12
    jmp exit

exit:
    mov eax, 0x77
    int 0x80 ; call sigreturn()
    ud2 ; should never reach

align 16

signal_trampoline_info:
    dd 0xdeadbeef ; signal number
    dd 0          ; siginfo_t*
    dd 0          ; ucontext*
    dd 0          ; handler
    dd 0          ; flags
