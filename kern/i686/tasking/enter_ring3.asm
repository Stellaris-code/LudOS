global enter_ring3

USER_CODE_SELECTOR equ 0x18
USER_DATA_SELECTOR equ 0x18

enter_ring3:
    push ebp
    mov  ebp, esp

    cli ; disable instructions

    pop dword eax ; page directory
    mov cr3, eax ; set user page directory

    pop dword eax ; user_eip
    pop dword ecx ; user_esp

    ;;;;;;;;;;;;

    push dword USER_DATA_SELECTOR | 0x3 ; ss
    push ecx ; esp

    pushfd
    pop ecx ; eflags
    or ecx, 0011_0010_0000_0000b ; set IF and set IOPL to 3

    push ecx ; user_eflags
    push dword USER_CODE_SELECTOR | 0x3 ; user_cs
    push dword eax ; user_eip

    mov ax, USER_DATA_SELECTOR | 0x3
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    iret ;  to user land !

    leave
    ret
