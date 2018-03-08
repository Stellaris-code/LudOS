cmake_minimum_required(VERSION 2.8)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ${ARCH})
set(CMAKE_CROSSCOMPILING 1)

set(CMAKE_C_COMPILER_WORKS 1) # Les test compilateur de CMake utilisent un flag, -rdynamic pas disponible sur le cross compiler qu'on a
set(CMAKE_CXX_COMPILER_WORKS 1)

project(TestPrograms)

set(ARCH i686)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
set(CMAKE_ASM_NASM_COMPILER "/usr/bin/nasm")
enable_language(ASM_NASM)

file(GLOB_RECURSE DIV0_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "div_zero/*.cpp" "div_zero/*.hpp")
set(COMMON_SRCS "layout.ld" "start.asm")

add_executable(DivZero ${COMMON_SRCS} ${DIV0_SRCS})

set_target_properties(DivZero PROPERTIES LINK_FLAGS
  "-T ${CMAKE_CURRENT_SOURCE_DIR}/layout.ld  -Xlinker")
set(CMAKE_ASM_NASM_LINK_EXECUTABLE "${ARCH}-elf-gcc -melf_i386 -nodefaultlibs -nostdlib -nostartfiles -T ${CMAKE_CURRENT_SOURCE_DIR}/layout.ld <OBJECTS> -o <TARGET>.bin")
set(CMAKE_CXX_FLAGS "-nostdlib -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs")
