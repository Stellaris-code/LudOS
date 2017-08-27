cmake_minimum_required(VERSION 2.8)

set(ARCH i686)
set(PLATFORM pc)

add_definitions(-DARCH_${ARCH})

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ${ARCH})
set(CMAKE_CROSSCOMPILING 1)

set(CMAKE_C_COMPILER_WORKS 1) # Les test compilateur de CMake utilisent un flag, -rdynamic pas disponible sur le cross compiler qu'on a
set(CMAKE_CXX_COMPILER_WORKS 1)

project(LudOS)

set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(profile_detection)

LOAD_PROFILE("${ARCH}" "${PLATFORM}")

set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
set(CMAKE_ASM_NASM_COMPILER "/usr/bin/nasm")
enable_language(ASM_NASM)

file(GLOB KERN_TERMINAL_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "kern/terminal/*.cpp")
file(GLOB KERN_BIOS_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "kern/bios/*.cpp")
file(GLOB KERN_MB_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "kern/multiboot/*.cpp")
file(GLOB KERN_BASE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "kern/*.cpp")
file(GLOB CPP_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "kern/cpp_runtime/*.cpp")
file(GLOB_RECURSE LIBC_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "libc/*.c" "libc/*.cpp")
file(GLOB_RECURSE LIBC_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "libc/*.h" "libc/*.hpp")
file(GLOB_RECURSE HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.hpp" "*.h" "*.tpp")
file(GLOB_RECURSE LD_SCRIPTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ld")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/kern")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libc")

set(GENERIC_SRCS ${KERN_TERMINAL_SOURCES} ${CPP_SOURCES} ${KERN_BIOS_SOURCES} ${KERN_MB_SOURCES} ${KERN_BASE_SOURCES})

add_executable(${PROJECT_NAME} ${PLATFORM_SRCS} ${ISA_SRCS} ${GENERIC_SRCS} ${HEADERS} ${LD_SCRIPTS})

set(CMAKE_C_FLAGS "${ISA_C_FLAGS} ${PLATFORM_C_FLAGS}  -m32 -std=c11 -ftree-vectorize -Wall -Wextra -ffreestanding -fno-builtin -nostdlib -finline-functions -fno-stack-protector -fomit-frame-pointer -Werror=implicit-function-declaration -mno-sse")
set(CMAKE_CXX_FLAGS "${ISA_CXX_FLAGS} ${PLATFORM_CXX_FLAGS}  -m32 -std=c++17 -ftree-vectorize -Wall -Wextra -ffreestanding -fno-builtin -nostdlib -finline-functions -fno-stack-protector -fomit-frame-pointer -Werror=implicit-function-declaration -mno-sse -fno-exceptions -fno-rtti")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS
  "-T ${PLATFORM_LAYOUT} -N ${ISA_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS}")
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".bin")
set(CMAKE_ASM_NASM_LINK_EXECUTABLE "${ARCH}-elf-gcc -T ${PLATFORM_LAYOUT} -N ${ISA_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS} <OBJECTS> -o <TARGET>.bin")

add_library(libc ${LIBC_SOURCES} ${LIBC_HEADERS})
target_compile_definitions(libc
    PUBLIC __is_libk)

target_link_libraries(${PROJECT_NAME} libc)


add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "./multiboot-check.sh" $<TARGET_FILE:${PROJECT_NAME}>
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/tools")
