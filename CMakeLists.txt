cmake_minimum_required(VERSION 2.8)

set(ARCH i686)
set(PLATFORM pc)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ${ARCH})
set(CMAKE_CROSSCOMPILING 1)

set(CMAKE_C_COMPILER_WORKS 1)
set(CMAKE_CXX_COMPILER_WORKS 1)

project(LudOS)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(profile_detection)

LOAD_PROFILE("${ARCH}" "${PLATFORM}")

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
set(CMAKE_ASM_NASM_COMPILER "/usr/bin/nasm")
enable_language(ASM_NASM)

add_executable(${PROJECT_NAME} ${PLATFORM_SRCS} ${ISA_SRCS} ${GENERIC_SRCS})

set(CMAKE_C_FLAGS "${ISA_C_FLAGS} ${PLATFORM_C_FLAGS}  -m32 -std=c11 -Wall -Wextra -ffreestanding -fno-builtin -nostdlib -nostdinc -finline-functions -fno-stack-protector -fomit-frame-pointer -Werror=implicit-function-declaration -mno-sse")
set(CMAKE_CXX_FLAGS "${ISA_CXX_FLAGS} ${PLATFORM_CXX_FLAGS}  -m32 -std=c++11 -Wall -Wextra -ffreestanding -fno-builtin -nostdlib -nostdinc -finline-functions -fno-stack-protector -fomit-frame-pointer -Werror=implicit-function-declaration -mno-sse -fno-exceptions -fno-rtti")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS
  "-T ${PLATFORM_LAYOUT} -N ${ISA_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS}")
set(CMAKE_ASM_NASM_LINK_EXECUTABLE "${ARCH}-elf-gcc -T ${PLATFORM_LAYOUT} -N ${ISA_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS} <OBJECTS> -o <TARGET>.bin")

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "${CMAKE_SOURCE_DIR}/tools/multiboot-check.sh" "$<TARGET_FILE:${PROJECT_NAME}>.bin")
