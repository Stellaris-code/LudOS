%macro ADD_SYSCALL_GATE 2
global do_%1_syscall
do_%1_syscall:
    ; save preserved registers on the stack
    push ebp
    push edi
    push esi
    push ebx

    mov eax, [esp+0x1c] ; arguments

    mov ebx, [eax]
    mov ecx, [eax+0x4]
    mov edx, [eax+0x8]
    mov esi, [eax+0xc]
    mov edi, [eax+0x10]
    mov ebp, [eax+0x14]

    mov eax, [esp+0x14] ; syscall number

    int %2 ; do syscall

    pop ebx
    pop esi
    pop edi
    pop ebp

    ret
%endmacro

ADD_SYSCALL_GATE ludos, 0x70
ADD_SYSCALL_GATE linux, 0x80
