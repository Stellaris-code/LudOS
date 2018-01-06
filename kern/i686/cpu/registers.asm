global update_registers
extern handle_registers_request

get_eip: mov eax, [esp]
         ret

update_registers:
    mov eax, [esp]

    push ss
    push eax
    pushfd
    push cs

    mov [old_eax], eax
    call get_eip
    push eax

    ; int_no, err_code
    push dword 0
    push dword 0

    ; Store general purpose
    push edi
    push esi
    push ebp
    push ebx
    push edx
    push ecx
    push dword [old_eax]

    ; Store segments
    push ds
    push es
    push fs
    push gs

    ; Push stack pointer
    push esp
    ; Call handler
    call handle_registers_request
    ; Set stack pointer to returned value
    mov esp, eax
    ; Restore segments
    pop ebx
    pop ebx
    pop ebx
    pop ebx

    ; Restore general purpose
    pop ebx
    pop ebx
    pop ebx
    pop ebx
    pop ebx
    pop ebx
    pop ebx

    ; int_no, err_code
    pop ebx
    pop ebx

    pop ebx
    pop ebx
    pop ebx
    pop ebx
    pop ebx

    ret

align 4
old_eax:    dd 0
