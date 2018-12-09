BITS 32

extern main

section .text
global _start

extern start_ctors
extern end_ctors
extern start_dtors
extern end_dtors

_start:
    ; Call ctors
    mov ebx, start_ctors

.init_loop:
    cmp ebx, end_ctors
    jge .init_end
    call [ebx]
    lea ebx, [ebx+4]
    jmp .init_loop

.init_end:

    call main
    ; main has returned, eax is return value
    push eax

    ; Call dtors
    mov ebx, start_dtors

.dtor_loop:
    cmp ebx, end_dtors
    jge .dtor_end
    call [ebx]
    lea ebx, [ebx+4]
    jmp .dtor_loop

.dtor_end:

    ; call exit syscall
    mov eax, 1
    pop ebx ; pop back main return value
    int 0x80

_wait:
    jmp  _wait    ; loop forever

section .header
extern last_allocated_page_sym
magic: db "LUDOSBIN"
last_allocated_page: dd last_allocated_page_sym
