global has_sse
global enable_sse

has_sse:
    push ebx
    push ecx
    push edx

    mov eax, 0x1
    cpuid
    test edx, 1<<25

    pop edx
    pop ecx
    pop ebx

    jnz .has_sse
    mov eax, 0 ; no sse
    ret

.has_sse:
    mov eax, 1
    ret


enable_sse:
    mov eax, cr0
    and ax, 0xFFFB		;clear coprocessor emulation CR0.EM
    or ax, 0x2			;set coprocessor monitoring  CR0.MP
    mov cr0, eax
    mov eax, cr4
    or ax, 3 << 9		;set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
    mov cr4, eax
    ret
