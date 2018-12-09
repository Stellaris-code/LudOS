extern tss

global task_switch
task_switch:
    ; Structure :
    ; eax...edi
    ; eflags
    ; eip
    pushfd
    pushad ; eax to edi including esp

    cli

    mov dword [esp+0x0c], 0xcafebabe ; set dummy_esp to a sentinel value

    mov eax, [esp+0x28] ; *from
    mov [eax], esp ; from->esp = esp
    mov eax, [esp+0x2c] ; *to
    mov esp, [eax] ; esp = to->esp
    mov dword [tss+0x4], esp ; update tss field

    popad
    popfd

    ret ; [esp] contains the return address
