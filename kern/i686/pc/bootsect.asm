global _start                           ; making entry point visible to linker

extern kmain                            ; kmain is defined in kmain.cpp

extern start_ctors                      ; beginning and end
extern end_ctors                        ; of the respective
extern start_dtors                      ; ctors and dtors section,
extern end_dtors                        ; declared by the linker script

extern __cxa_finalize

section .text

; reserve initial kernel stack space
STACKSIZE equ 0x10000                    ; that's 64k.

_start:
    mov  esp, stack + STACKSIZE         ; set up the stack

    mov [magic], eax
    ;add ebx, KERNEL_VIRTUAL_BASE ; make the address virtual
    mov [mbd_info], ebx

; 0x0024a04e
    mov  ebx, start_ctors               ; call the constructors
    jmp  .ctors_until_end
.call_constructor:
    call [ebx]
    add  ebx,4
.ctors_until_end:
    cmp  ebx, end_ctors
    jb   .call_constructor

    push dword [mbd_info]
    push dword [magic]

    call kmain                          ; call kernel proper

    mov  ebx, end_dtors                 ; call the destructors
    jmp  .dtors_until_end
.call_destructor:
    sub  ebx, 4
    call [ebx]
.dtors_until_end:
    cmp  ebx, start_dtors
    ja   .call_destructor

    sub esp, 4
    mov [esp], dword 0x0

    call __cxa_finalize

    add esp, 4

    cli
hang:
    hlt                                 ; halt machine should kernel return
    jmp  hang

section .bss

align 4
magic:      resd 1
mbd_info:   resd 1
align 4
stack:      resb STACKSIZE                   ; reserve 16k stack on a doubleword boundary
