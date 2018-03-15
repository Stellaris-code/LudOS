global enter_ring3
global test_task

USER_CODE_SELECTOR equ 0x18
USER_DATA_SELECTOR equ 0x20

test_task:
    xchg bx, bx
    xor eax, eax
    div eax

enter_ring3:
    push ebp
    mov  ebp, esp

    cli ; disable instructions

    mov ecx, [esp+12] ; user_eip
    mov edx, [esp+8] ; user_esp

    ;;;;;;;;;;;;

    pushfd
    pop eax ; eflags
    or eax, 0011_0010_0000_0000b ; set IF and set IOPL to 3

    push dword USER_DATA_SELECTOR | 0x3 ; ss
    push edx ; esp

    push eax ; user_eflags
    push dword USER_CODE_SELECTOR | 0x3 ; user_cs
    push ecx ; user_eip

    mov eax, USER_DATA_SELECTOR | 0x3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    iret ;  to user land !

    leave
    ret
