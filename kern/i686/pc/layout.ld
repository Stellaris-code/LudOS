ENTRY(_start)

SECTIONS
{
    . = 0xC0000000 + 1M; /* + 1M */

    .boot ALIGN(0x1000) : AT(ADDR(.boot) - 0xC0000000)
    {
        mbd = .;
        *(.multiboot_header)
    }

    _load_start = .;

    .text ALIGN(0x1000) : AT(ADDR(.text) - 0xC0000000)
    {
        *(.text)
        *(.gnu.linkonce.t*)
    }

    .rodata ALIGN(0x1000) : AT(ADDR(.rodata) - 0xC0000000)
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

    /* For stack unwinding (exception handling)  */
    .eh_frame_hdr ALIGN(0x8): AT(ADDR(.eh_frame_hdr) - 0xC0000000)
    {
      KEEP(*(.eh_frame_hdr*))
    }
    .eh_frame ALIGN(0x8): AT(ADDR(.eh_frame) - 0xC0000000)
    {
      PROVIDE (__eh_frame_start = .);
      KEEP(*(.eh_frame))
      LONG (0);
    }

    .gcc_except_table : AT(ADDR(.gcc_except_table) - 0xC0000000)
    {
      *(.gcc_except_table)
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


    . = ALIGN(0x1000);

    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
    }   


    kernel_virtual_end = .;
    kernel_physical_end = . - 0xC0000000;
}
