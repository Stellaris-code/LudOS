MAGIC       equ    0xE85250D6           ; 'magic number' lets bootloader find the header

section .multiboot_header
header_start:
    dd MAGIC
    dd 0
    dd header_end-header_start
    ;checksum
    dd 0x100000000-(0xE85250D6+0+(header_end-header_start))

    ;optional multiboot tags
    ;none here

    ;end tag
    dw 0                        ;type
    dw 0                        ;flags
    dd 8                        ;size
header_end:
