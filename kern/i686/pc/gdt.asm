global gdt_flush

gdt_flush:
    mov eax, [esp+4] ; get address of gdt table
    lgdt [eax] ; load it

    jmp 0x08:.flush   ; 0x08 is the offset to our code segment: Far jump! Update the segment register
.flush:
    mov     ax, 0x10 ; 0x10 points at the new data selector
    mov     ds, ax
    mov     es, ax
    mov     fs, ax
    mov     gs, ax
    mov     ss, ax
    ret
