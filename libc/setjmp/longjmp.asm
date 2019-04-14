global _longjmp
global longjmp

_longjmp:
longjmp:
        mov  edx, [esp+4]
        mov  eax, [esp+8]
        test    eax,eax
        jnz lbl
        inc     eax
lbl:
        mov   ebx, [edx]
        mov   esi, [edx+4]
        mov   edi, [edx+8]
        mov   ebp, [edx+12]
        mov   ecx, [edx+16]
        mov   esp, ecx
        mov   ecx, [edx+20]
jmp ecx
