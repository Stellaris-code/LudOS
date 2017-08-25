MAGIC       equ    0xE85250D6           ; 'magic number' lets bootloader find the header

[EXTERN _load_start]
[EXTERN _load_end]
[EXTERN _bss_end]
[EXTERN _start]

section .multiboot_header
header_start:
    dd MAGIC
    dd 0
    dd header_end-header_start
    ;checksum
    dd 0x100000000-(0xE85250D6+0+(header_end-header_start))

    align 8

    ;Info requests
    info_request_start:
    dw 1
    dw 0
    dd info_request_end-info_request_start
    dd 4 ; basic meminfo
    dd 1 ; command line
    dd 5 ; bios boot device
    dd 6 ; memory map
    dd 2 ; bootloader name
    info_request_end:

    align 8

    ;Entry point
    dw 4
    dw 0
    dd 12
    dd _start

    align 8

    ;end tag
    dw 0                        ;type
    dw 0                        ;flags
    dd 8                        ;size
header_end:
