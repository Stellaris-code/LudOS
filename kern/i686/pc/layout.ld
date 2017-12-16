ENTRY(_start)

SECTIONS
{
    . = 0xE0100000; /* + 1M */

    .boot ALIGN(0x1000) : AT(ADDR(.boot) - 0xE0000000)
    {
        mbd = .;
        *(.multiboot_header)
    }

    _load_start = .;

    .text ALIGN(0x1000) : AT(ADDR(.text) - 0xE0000000)
    {
        *(.text)
        *(.gnu.linkonce.t*)
    }

    .rodata ALIGN(0x1000) : AT(ADDR(.rodata) - 0xE0000000)
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

    .data ALIGN(0x1000) : AT(ADDR(.data) - 0xE0000000)
    {
        data_seg = .;
        *(.data)
        *(.gnu.linkonce.d*)
    }

    _load_end = .;

    .bss : AT(ADDR(.bss) - 0xE0000000)
    {
        _bss_start = .;
        *(COMMON)
        *(.bss)
        *(.gnu.linkonce.b*)
        _bss_end = .;
    }


    . = ALIGN(0x1000);

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }   


    kernel_virtual_end = .;
    kernel_physical_end = . - 0xE0000000;
}
