OUTPUT_FORMAT("binary")    /* output flat binary */
ENTRY(_start)

SECTIONS
{
    . = 0;

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
}
