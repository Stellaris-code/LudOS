ENTRY(_start)

SECTIONS
{
    . = 0x00100000; /* + 1M */

    .boot ALIGN(0x1000) : AT(ADDR(.boot))
    {
        mbd = .;
        *(.multiboot_header)
    }

    _load_start = .;

    .text ALIGN(0x1000) : AT(ADDR(.text))
    {
        *(.text)
        *(.gnu.linkonce.t*)
    }

    .rodata ALIGN(0x1000) : AT(ADDR(.rodata))
    {
        start_ctors = .;
        *(SORT(.ctors*))  /* Note the "SORT" */
        end_ctors = .;

        start_dtors = .;
        *(SORT(.dtors*))
        end_dtors = .;

        *(.rodata*)
        *(.gnu.linkonce.r*)
    }

    .data ALIGN(0x1000) : AT(ADDR(.data))
    {
        data_seg = .;
        *(.data)
        *(.gnu.linkonce.d*)
    }

    _load_end = .;

    .bss : AT(ADDR(.bss))
    {
        _bss_start = .;
        *(COMMON)
        *(.bss)
        *(.gnu.linkonce.b*)
        _bss_end = .;
    }


    kernel_virtual_end = .;
    kernel_physical_end = .;

    . = ALIGN(0x1000);

    /DISCARD/ :
    {
        *(.comment)
/*        *(.eh_frame) */
    }
}
