global switch_stacks
switch_stacks:
    lea eax, [esp+0x8] ; load former esp value (minus the return address and the parameter) to eax
    mov ecx, [esp] ; put the return address in ecx

    mov esp, [esp+0x4] ; load the new esp value

    jmp [ecx] ; jump to the return address
