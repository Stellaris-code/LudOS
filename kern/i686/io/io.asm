global insw
global outsw

insw:
    push ebp
    mov  ebp, esp

    mov  ecx, DWORD [ebp+16] ;  number of repetitions
    mov  edi, DWORD [ebp+12] ; destination address
    mov   dx, WORD  [ebp+8]  ; io port

    cld

    rep insw

    leave
    ret

outsw:
    push ebp
    mov  ebp, esp

    mov  ecx, DWORD [ebp+16] ;  number of repetitions
    mov  esi, DWORD [ebp+12] ; source address
    mov   dx, WORD  [ebp+8]  ; io port

    cld

    rep outsw

    leave
    ret
