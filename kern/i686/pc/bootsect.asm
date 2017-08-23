global _start                           ; making entry point visible to linker

extern kmain                            ; kmain is defined in kmain.cpp

extern start_ctors                      ; beginning and end
extern end_ctors                        ; of the respective
extern start_dtors                      ; ctors and dtors section,
extern end_dtors                        ; declared by the linker script

section .text

; reserve initial kernel stack space
STACKSIZE equ 0x4000                    ; that's 16k.

_start:
    mov  esp, stack + STACKSIZE         ; set up the stack

    mov  ebx, start_ctors               ; call the constructors
    jmp  .ctors_until_end
.call_constructor:
    call [ebx]
    add  ebx,4
.ctors_until_end:
    cmp  ebx, end_ctors
    jb   .call_constructor

    call kmain                          ; call kernel proper

    mov  ebx, end_dtors                 ; call the destructors
    jmp  .dtors_until_end
.call_destructor:
    sub  ebx, 4
    call [ebx]
.dtors_until_end:
    cmp  ebx, start_dtors
    ja   .call_destructor

    cli
.hang:
    hlt                                 ; halt machine should kernel return
    jmp  .hang

section .bss

align 4
magic: resd 1
mbd:   resd 1
stack: resb STACKSIZE                   ; reserve 16k stack on a doubleword boundary
