OUTPUT_FORMAT("binary")    /* output flat binary */
ENTRY(_start)

SECTIONS
{
    . = 0;

    .header ALIGN(4):
    {
        *(.header)
    }

    .text ALIGN(4):
    {
        *(.text)           /* include all other .text sections */
    }

    .data ALIGN(4):
    {
        *(.data)
    }

    .rodata ALIGN(4):
    {
        *(.rodata*)
    }

    .bss ALIGN(4):
    {
        *(.bss*)
    }

    . = ALIGN(4K);
    last_allocated_page_sym = .;
}
