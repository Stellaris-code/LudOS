global _start                           ; making entry point visible to linker
global kernel_stack_bottom
global kernel_stack_top

extern kmain                            ; kmain is defined in kmain.cpp

extern start_ctors                      ; beginning and end
extern end_ctors                        ; of the respective
extern start_dtors                      ; ctors and dtors section,
extern end_dtors                        ; declared by the linker script

extern _bss_start
extern _bss_end

extern __cxa_finalize

%include "defs.asm"

KERNEL_PAGE_NUMBER equ (KERNEL_VIRTUAL_BASE >> 22)

section .data
align 0x1000
BootPageDirectory:
    ; This page directory entry identity-maps the first 4MB of the 32-bit physical address space.
    ; All bits are clear except the following:
    ; bit 7: PS The kernel page is 4MB.
    ; bit 1: RW The kernel page is read/write.
    ; bit 0: P  The kernel page is present.
    ; This entry must be here -- otherwise the kernel will crash immediately after paging is
    ; enabled because it can't fetch the next instruction! It's ok to unmap this page later.
    dd 0x00000083
    times (KERNEL_PAGE_NUMBER - 1) dd 0                 ; Pages before kernel space.
    ; This page directory entry defines 8 4MB pages containing the kernel.
    dd 0x00000083
    dd 0x00400083
    dd 0x00800083
    dd 0x00c00083
    dd 0x01000083
    dd 0x01400083
    dd 0x01800083
    dd 0x01c00083
    dd 0x02000083
    dd 0x02400083
    dd 0x02800083
    dd 0x02c00083
    times (1024 - KERNEL_PAGE_NUMBER - 12) dd 0  ; Pages after the kernel image.

kernel_stack_top equ kernel_stack_bottom + STACKSIZE

section .text

; reserve initial kernel stack space
STACKSIZE equ 0x4000                   ; that's 16k.

_start:
    ; clear bss section
;    lea edi, [_bss_end]
;    lea esi, [_bss_start]
;_bss_loop:
;    mov DWORD [esi], 0
;    lea esi, [esi+4]
;    cmp esi, edi
;    jb _bss_loop

    ; NOTE: Until paging is set up, the code must be position-independent and use physical
    ; addresses, not virtual ones!
    mov ecx, (BootPageDirectory - KERNEL_VIRTUAL_BASE)
    mov cr3, ecx                                        ; Load Page Directory Base Register.

    mov ecx, cr4
    or ecx, 0x00000010                          ; Set PSE bit in CR4 to enable 4MB pages.
    mov cr4, ecx

    mov ecx, cr0
    or ecx, 0x80000000                          ; Set PG bit in CR0 to enable paging.
    mov cr0, ecx

    ; Start fetching instructions in kernel space.
    ; Since eip at this point holds the physical address of this command (approximately 0x00100000)
    ; we need to do a long jump to the correct virtual address of StartInHigherHalf which is
    ; approximately 0xC0100000.
    lea ecx, [higher_half_start]
    jmp ecx

higher_half_start:
    mov dword [BootPageDirectory], 0
    invlpg [0]

    mov  esp, kernel_stack_bottom + STACKSIZE         ; set up the stack

    mov [magic], eax
    add ebx, KERNEL_VIRTUAL_BASE
    mov [mbd_info], ebx

    sub esp, 8 ; align stack to 16-byte boundary

    push dword [mbd_info]
    push dword [magic]

.kmain_call:
    call kmain                          ; call kernel proper

    cli
hang:
    hlt                                 ; halt machine should kernel return
    jmp  hang

section .bss

align 0x1000
kernel_stack_bottom:      resb STACKSIZE                   ; reserve 16k stack on a doubleword boundary
magic:      resd 1
mbd_info:   resd 1
