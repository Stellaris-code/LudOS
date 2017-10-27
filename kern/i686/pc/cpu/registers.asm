global update_registers
extern handle_registers_request

update_registers:
    ; Store general purpose
    push edi
    push esi
    push ebp
    push ebx
    push edx
    push ecx
    push eax

    ; Store segments
    push ds
    push es
    push fs
    push gs

    ; Switch to kernel segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; Push stack pointer
    push esp
    ; Call handler
    call handle_registers_request
    ; Set stack pointer to returned value
    mov esp, eax
    ; Restore segments
    pop gs
    pop fs
    pop es
    pop ds

    ; Restore general purpose
    pop eax
    pop ecx
    pop edx
    pop ebx
    pop ebp
    pop esi
    pop edi


    ret

