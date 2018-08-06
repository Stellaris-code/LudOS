global enter_ring3
global test_task

test_task:
    xchg bx, bx
    xor eax, eax
    div eax

enter_ring3:
    push ebp
    mov  ebp, esp

    cli ; disable instructions

    mov esi, [esp+0x8] ; reg_frame

    ;;;;;;;;;;;;

    pushfd
    pop eax ; eflags
    or eax, 0011_0010_0000_0000b ; set IF and set IOPL to 3

    push dword [esi+0x44] ; ss
    push dword [esi+0x40] ; esp

    push eax ; user_eflags
    push dword [esi+0x38] ; user_cs
    push dword [esi+0x34] ; eip

    mov eax, [esi+0x44] ; ds
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov eax, [esi+0x10] ; restore gprs
    mov ecx, [esi+0x14]
    mov edx, [esi+0x18]
    mov ebx, [esi+0x1c]
    mov ebp, [esi+0x20]
    mov esi, [esi+0x24]
    mov edi, [esi+0x28]


    iret ;  to user land !
