ENTRY(_start)

SECTIONS
{
    . = 1M;

    .boot ALIGN(0x1000) :
    {
        mbd = .;
        *(.multiboot_header)
    }

    _load_start = .;

    .text ALIGN(0x1000) :
    {
        *(.text)
        *(.gnu.linkonce.t*)
    }

    .rodata ALIGN(0x1000) :
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

    .data ALIGN(0x1000) :
    {
        *(.data)
        *(.gnu.linkonce.d*)
    }

    _load_end = .;

    .bss :
    {
        _bss_start = .;
        *(COMMON)
        *(.bss)
        *(.gnu.linkonce.b*)
        _bss_end = .;
    }

    kernel_end = .;

    /DISCARD/ :
    {
        *(.comment)
/*        *(.eh_frame)  discard this, unless you are implementing runtime support for C++ exceptions. */
    }
}
