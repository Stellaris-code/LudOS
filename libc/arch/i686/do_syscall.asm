%macro ADD_SYSCALL_GATE 2
global do_%1_syscall
do_%1_syscall:

    ; First, save preserved registers in order to not confuse the c compiler

    mov [ebx_save], ebx
    mov [esi_save], esi
    mov [edi_save], edi
    mov [ebp_save], ebp

    mov ebx, [esp+0xc]
    mov ecx, [esp+0x10]
    mov edx, [esp+0x14]
    mov esi, [esp+0x18]
    mov edi, [esp+0x1c]
    mov ebp, [esp+0x20]

    mov eax, [esp+0x4] ; syscall number

    int %2 ; LudOS syscall

    ; Restore saved registers

    mov ebx, [ebx_save]
    mov esi, [esi_save]
    mov edi, [edi_save]
    mov ebp, [ebp_save]

    ret
%endmacro

ADD_SYSCALL_GATE ludos, 0x70
ADD_SYSCALL_GATE linux, 0x80

section .bss
align 4
ebx_save: resd 1
esi_save: resd 1
edi_save: resd 1
ebp_save: resd 1
