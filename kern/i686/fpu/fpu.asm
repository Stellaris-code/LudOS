global check_fpu_presence
global setup_fpu

check_fpu_presence:
    MOV EDX, CR0                            ; Start probe, get CR0
    MOV CR0, EDX                            ; store control word
    FNINIT                                  ; load defaults to FPU
    FNSTSW [.testword]                      ; store status word
    CMP word [.testword], 0                 ; compare the written status with the expected FPU state
    JNE .nofpu                              ; jump if the FPU hasn't written anything (i.e. it's not there)
    JMP .hasfpu

.nofpu:
    mov eax, 0 ;false
    ret

.hasfpu:
    mov eax, 1 ;true
    ret

.testword: DW 0x55AA                    ; store garbage to be able to detect a change


setup_fpu:
    mov edx, cr0
    or edx, 0001_0001b
    and edx, 0xFFFFFFFB
    mov cr0, edx

    mov edx, cr4
    ;or edx, 0100_0000_0110_0000_0000b ; Check SSE first ! causes #GPF otherwise
    mov cr4, edx

    fninit
    ret
