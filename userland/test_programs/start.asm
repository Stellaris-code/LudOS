BITS 32

extern main

section .text
global _start

_start:
    ; push argv
    ; push argc
    call main
    ; main has returned, eax is return value

    ; call exit syscall
    xor eax, eax
    int 0x80

_wait:
    hlt
    jmp  _wait    ; loop forever
