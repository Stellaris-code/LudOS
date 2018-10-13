global task_switch
task_switch:
    ; Structure :
    ; eax...edi
    ; eflags
    ; eip
    cli
    push dword [esp] ; push the return eip
    pushfd
    pushad ; eax to edi including esp

    mov esp, [esp+40] ; set esp to the other task's esp value
    popad
    popfd

    sti
    jmp [esp]



global do_switch_same
do_switch_same:
    cli ; disable interrupts

    mov esi, [esp+0x4] ; reg_frame

    ;;;;;;;;;;;;

    push eax

    mov eax, [esi+0x38] ; eip
    mov [ret_eip], eax
    mov eax, [esi+0x40] ; eflags
    mov [saved_eflags], eax

    pop eax

    mov esp, [esi+0x44]
    mov ss, [esi+0x48]

    mov ds, [esi+0xc]
    mov es, [esi+0x8]
    mov fs, [esi+0x4]
    mov gs, [esi+0x0]

    mov edi, [esi+0x10] ; restore gprs
    mov ebp, [esi+0x18]
    mov ebx, [esi+0x20]
    mov edx, [esi+0x24]
    mov ecx, [esi+0x28]
    mov eax, [esi+0x2c]

    mov esi, [esi+0x14]

    push dword [saved_eflags]
    popfd ; restore flags

    jmp [ret_eip] ; jump to execution

section .bss
saved_eflags: resd 2
ret_eip:    resd 1
