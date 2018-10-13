global do_switch_inter

do_switch_inter:
    cli ; disable instructions

    mov esp, [esp+0x4] ; reg_frame

    ;;;;;;;;;;;;
    pop gs ; segment registers
    pop fs
    pop es
    pop ds

    popad ; restore gprs

    add esp, 8 ; int_no and err_code

    iret ;  to user land !
