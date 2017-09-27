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
        *(.gnu.warning)
        KEEP (*(.init))
        KEEP (*(.fini))
    }

    .eh_frame_hdr : { *(.eh_frame_hdr) }
    .eh_frame : { *(.eh_frame) }
    .gcc_except_table : {*(.gcc_except_table)}

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


    . = ALIGN(0x1000);

    /DISCARD/ :
    {
        *(.comment)
    }   

    /* DWARF debug sections.
       Symbols in the DWARF debugging sections are relative to the beginning
       of the section so we begin them at 0.  */
    /* DWARF 1 */
    .debug          0 : { *(.debug) }
    .line           0 : { *(.line) }
    /* GNU DWARF 1 extensions */
    .debug_srcinfo  0 : { *(.debug_srcinfo) }
    .debug_sfnames  0 : { *(.debug_sfnames) }
    /* DWARF 1.1 and DWARF 2 */
    .debug_aranges  0 : { *(.debug_aranges) }
    .debug_pubnames 0 : { *(.debug_pubnames) }
    /* DWARF 2 */
    .debug_info     0 : { *(.debug_info .gnu.linkonce.wi.*) }
    .debug_abbrev   0 : { *(.debug_abbrev) }
    .debug_line     0 : { *(.debug_line) }
    .debug_frame    0 : { *(.debug_frame) }
    .debug_str      0 : { *(.debug_str) }
    .debug_loc      0 : { *(.debug_loc) }
    .debug_macinfo  0 : { *(.debug_macinfo) }
    /* SGI/MIPS DWARF 2 extensions */
    .debug_weaknames 0 : { *(.debug_weaknames) }
    .debug_funcnames 0 : { *(.debug_funcnames) }
    .debug_typenames 0 : { *(.debug_typenames) }
    .debug_varnames  0 : { *(.debug_varnames) }

    kernel_virtual_end = .;
    kernel_physical_end = .;
}
