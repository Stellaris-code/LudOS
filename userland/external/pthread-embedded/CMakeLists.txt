file(GLOB source_files "*.c" "platform/ludos/*.c")
file(GLOB header_files "*.h" "platform/ludos/*.h")

include_directories(".")
include_directories("platform/ludos")

add_library(pte ${header_files} ${source_files})
