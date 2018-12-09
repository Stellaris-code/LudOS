ENTRY(_start)

SECTIONS
{
    . = 0xC0000000 + 1M;

    _load_start = .;

    .mb_section ALIGN(0x4) : AT(ADDR(.mb_section) - 0xC0000000)
    {
        mbd = .;
        *(.multiboot_header)
    }
    
    .text ALIGN(0x1000) : AT(ADDR(.text) - 0xC0000000)
    {
        *(.text)
        *(.gnu.linkonce.t*)
    }

    .rodata ALIGN(0x1000) : AT(ADDR(.rodata) - 0xC0000000)
    {
        start_ctors = .;
        KEEP (*(SORT(.ctors*)))
        end_ctors = .;

        start_dtors = .;
        KEEP(*(SORT(.dtors*)))
        end_dtors = .;

        *(.rodata*)
        *(.gnu.linkonce.r*)
    }

    .data ALIGN(0x1000) : AT(ADDR(.data) - 0xC0000000)
    {
        data_seg = .;
        *(.data)
        *(.gnu.linkonce.d*)
    }

    _load_end = .;

    .bss : AT(ADDR(.bss) - 0xC0000000)
    {
        _bss_start = .;
        *(COMMON)
        *(.bss)
        _bss_end = .;
    }

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }   


    kernel_virtual_end = .;
    kernel_physical_end = . - 0xC0000000;
}
