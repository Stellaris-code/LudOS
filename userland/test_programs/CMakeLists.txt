cmake_minimum_required(VERSION 2.8)

set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR ${ARCH})
set(CMAKE_CROSSCOMPILING 1)

set(CMAKE_C_COMPILER_WORKS 1) # Les test compilateur de CMake utilisent un flag, -rdynamic pas disponible sur le cross compiler qu'on a
set(CMAKE_CXX_COMPILER_WORKS 1)

project(TestPrograms)

set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules)

include(add_test_program)

set(ARCH i686)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)
set(CMAKE_ASM_NASM_OBJECT_FORMAT elf32)
set(CMAKE_ASM_NASM_COMPILER "/usr/bin/nasm")
enable_language(ASM_NASM)

file(GLOB_RECURSE LIBC_SOURCES "${CMAKE_SOURCE_DIR}/libc/*.c" "${CMAKE_SOURCE_DIR}/libc/*.cpp" "${CMAKE_SOURCE_DIR}/libc/*.asm")
file(GLOB_RECURSE LIBC_HEADERS "${CMAKE_SOURCE_DIR}/libc/*.h" "${CMAKE_SOURCE_DIR}/libc/*.hpp")
file(GLOB_RECURSE LIBCPP_SOURCES "${CMAKE_SOURCE_DIR}/external/libc++/*.cpp" "${CMAKE_SOURCE_DIR}/external/libc++/*.c")
file(GLOB_RECURSE LIBCPP_HEADERS "${CMAKE_SOURCE_DIR}/external/libc++/*.hpp" "${CMAKE_SOURCE_DIR}/external/libc++/*.h")

include_directories("${CMAKE_SOURCE_DIR}/libc")
include_directories("${CMAKE_SOURCE_DIR}/external/libc++")

add_library(libc ${LIBC_SOURCES} ${LIBC_HEADERS})
target_compile_definitions(libc
    PUBLIC __is_libc)
target_compile_options(libc PUBLIC $<$<NOT:$<COMPILE_LANGUAGE:ASM_NASM>>:-w>)
target_compile_definitions(libc PUBLIC "-DLUDOS_USER")

add_library(user_libcpp ${LIBCPP_SOURCES} ${LIBCPP_HEADERS})
set_target_properties(user_libcpp PROPERTIES COMPILE_FLAGS "-w")
target_link_libraries(user_libcpp libc)
target_compile_definitions(user_libcpp PUBLIC "-D__LUDOS__ -DLUDOS_USER")

set(COMMON_SRCS "layout.ld" "start.asm" "init.cpp")

ADD_TEST_PROGRAM(DivZero div_zero)
ADD_TEST_PROGRAM(SyscallTest syscall_test)
ADD_TEST_PROGRAM(MoreOrLess more_or_less)
ADD_TEST_PROGRAM(ExecTest exec_test)
ADD_TEST_PROGRAM(Fork fork)
ADD_TEST_PROGRAM(Sleep sleep)
ADD_TEST_PROGRAM(ShittyShell shitty_shell)
ADD_TEST_PROGRAM(Tests tests)
ADD_TEST_PROGRAM(ShmTest shm_test)
ADD_TEST_PROGRAM(LeakTest leak_test)
ADD_TEST_PROGRAM(Cat cat)

set(CMAKE_ASM_NASM_LINK_EXECUTABLE "${ARCH}-elf-gcc -melf_i386 -nodefaultlibs -nostdlib -nostartfiles -T ${CMAKE_CURRENT_SOURCE_DIR}/layout.ld <OBJECTS> -o <TARGET>.bin")
set(CMAKE_CXX_FLAGS "-nostdlib -fno-pic -std=c++17 -fno-exceptions -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs")
set(CMAKE_C_FLAGS "-nostdlib -fno-pic -nostdinc -fno-builtin -fno-stack-protector -nostartfiles -nodefaultlibs")

set(CMAKE_CXX_LINK_EXECUTABLE "/usr/local/cross/bin/${ARCH}-elf-ld --build-id=none -T ${CMAKE_CURRENT_SOURCE_DIR}/layout.ld <OBJECTS> -o <TARGET> <LINK_LIBRARIES>-L/usr/tooLargeForHome/cross_src/build-gcc/i686-elf/libgcc/ -lgcc")
