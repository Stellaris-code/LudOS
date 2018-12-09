global switch_stacks
switch_stacks:
    push esi
    push edi

    mov eax, esp ; to return the former stack pointer

    mov esi, [esp+0x0c] ; former stack top
    mov edi, [esp+0x10] ; next   stack top

    cli ; do not treat interrupts with a corrupt stack !

    mov ecx, esi
    sub ecx, esp ; ecx contains the stack size

    ;sar ecx, 2 ; divide by 4 to get the dword count
    ;inc ecx

    std ; downwards

    rep movsb

    mov esp, edi ; esp now belongs to the next stack

    ; fix ebp too
    sub esi, ebp ; esi contains ebp's offset
    mov ebp, edi
    sub ebp, esi ; ebp = stack_top - offset

    pop edi
    pop esi

    sti

    cld
    ret

