global do_switch_inter
global do_switch_same

do_switch_inter:
    cli ; disable instructions

    mov esi, [esp+0x4] ; reg_frame

    ;;;;;;;;;;;;

    push dword [esi+0x44] ; ss
    push dword [esi+0x40] ; esp

    push dword [esi+0x3c] ; user_eflags
    push dword [esi+0x38] ; user_cs
    push dword [esi+0x34] ; eip

    mov ds, [esi+0xc]
    mov es, [esi+0x8]
    mov fs, [esi+0x4]
    mov gs, [esi+0x0]

    mov eax, [esi+0x10] ; restore gprs
    mov ecx, [esi+0x14]
    mov edx, [esi+0x18]
    mov ebx, [esi+0x1c]
    mov ebp, [esi+0x20]
    mov edi, [esi+0x28]
    mov esi, [esi+0x24]


    iret ;  to user land !


do_switch_same:
    cli ; disable instructions

    mov esi, [esp+0x4] ; reg_frame

    ;;;;;;;;;;;;

    push eax

    mov eax, [esi+0x34] ; eip
    mov [ret_eip], eax
    mov eax, [esi+0x3c] ; eflags
    mov [saved_eflags], eax

    pop eax

    mov esp, [esi+0x40]
    mov ss, [esi+0x44]

    mov ds, [esi+0xc]
    mov es, [esi+0x8]
    mov fs, [esi+0x4]
    mov gs, [esi+0x0]

    mov eax, [esi+0x10] ; restore gprs
    mov ecx, [esi+0x14]
    mov edx, [esi+0x18]
    mov ebx, [esi+0x1c]
    mov ebp, [esi+0x20]
    mov edi, [esi+0x28]

    mov esi, [esi+0x24]

    push dword [saved_eflags]
    popfd ; restore flags

    jmp [ret_eip] ; jump to execution

section .bss
saved_eflags: resd 2
ret_eip:    resd 1
