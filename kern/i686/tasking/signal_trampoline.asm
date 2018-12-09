global signal_trampoline
global signal_trampoline_info

%include "../kern/i686/defs.asm"

align 0x1000

signal_trampoline:
    cld ; clear direction flag before entering signal handler

    mov ebp, esp
    and esp, -16 ; align the stack to a 16B boundary

;    test dword [ebp+0x10], 0x4 ; test for SA_SIGINFO
;    jmp siginfo_path

;no_siginfo_path:
;    sub esp, 8 ; 16B boundary
;    push dword [ebp] ; the signal number
;    call [ebp+0xc] ; the handler
;    jmp exit

siginfo_path:
    push dword [ebp+0x14] ; ucontext*
    push dword [ebp+0x10] ; siginfo_t*
    push dword [ebp] ; the signal number
    call [ebp+0x4] ; the handler

exit:
    mov eax, 0x77
    int 0x80 ; call sigreturn()
    ud2 ; should never reach
