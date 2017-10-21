; Declare constants for the multiboot header.
MBALIGN  equ  1<<0              ; align loaded modules on page boundaries
MEMINFO  equ  1<<1              ; provide memory map
GFXINFO  equ  1<<2
FLAGS    equ  MBALIGN | MEMINFO ; this is the Multiboot 'flag' field
MAGIC    equ  0x1BADB002        ; 'magic number' lets bootloader find the header
CHECKSUM equ -(MAGIC + FLAGS)   ; checksum of above, to prove we are multiboot

; Declare a multiboot header that marks the program as a kernel. These are magic
; values that are documented in the multiboot standard. The bootloader will
; search for this signature in the first 8 KiB of the kernel file, aligned at a
; 32-bit boundary. The signature is in its own section so the header can be
; forced to be within the first 8 KiB of the kernel file.
section .multiboot_header
align 4
        dd MAGIC
        dd FLAGS
        dd CHECKSUM
        dd 0
        dd 0
        dd 0
        dd 0
        dd 0 ; addr fields
        dd 0 ; width
        dd 0 ; height
        dd 0 ; graphics mode
