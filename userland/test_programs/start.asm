BITS 32

extern main

section .text
global _start

_start:
    ; push argc
    ; push argv
    push ebx
    push eax

    call main
    ; main has returned, eax is return value

    ; call exit syscall
    xor eax, eax
    int 0x80

_wait:
    hlt
    jmp  _wait    ; loop forever

section .header
extern last_allocated_page_sym
magic: db "LUDOSBIN"
last_allocated_page: dd last_allocated_page_sym
