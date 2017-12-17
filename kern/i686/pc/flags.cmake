SET(ISA_LINKER_FLAGS "${ISA_LINKER_FLAGS}")

configure_file("${PROJECT_SOURCE_DIR}/kern/i686/pc/layout.ld.in" "${PROJECT_SOURCE_DIR}/kern/i686/pc/layout.ld")
configure_file("${PROJECT_SOURCE_DIR}/kern/i686/pc/defs.asm.in" "${PROJECT_SOURCE_DIR}/kern/i686/pc/defs.asm")
