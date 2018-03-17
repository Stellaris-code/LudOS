%macro ADD_SYSCALL_GATE 2
global do_%1_syscall
do_%1_syscall:

    ; First, save preserved registers in order to not confuse the c compiler

    mov [ebx_save], ebx
    mov [esi_save], esi
    mov [edi_save], edi
    mov [ebp_save], ebp

    mov eax, [esp+0xc]

    mov ebx, [eax]
    mov ecx, [eax+0x4]
    mov edx, [eax+0x8]
    mov esi, [eax+0xc]
    mov edi, [eax+0x10]
    mov ebp, [eax+0x14]

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

section .data
align 4
ebx_save: resd 1
esi_save: resd 1
edi_save: resd 1
ebp_save: resd 1
