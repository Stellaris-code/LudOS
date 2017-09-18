cmake_minimum_required(VERSION 2.8)

set(ARCH i686)
set(PLATFORM pc)

add_definitions(-DARCH_${ARCH} -DLudOS)

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

file(GLOB_RECURSE KERN_TERMINAL_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "kern/terminal/*.cpp")
file(GLOB_RECURSE KERN_BIOS_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "kern/bios/*.cpp")
file(GLOB_RECURSE KERN_MB_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "kern/multiboot/*.cpp")
file(GLOB_RECURSE KERN_BASE_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "kern/*.cpp" "kern/*.c")
file(GLOB_RECURSE ACPICA_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "external/acpica/*.c")
file(GLOB_RECURSE ACPICA_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "external/acpica/*.h")
file(GLOB_RECURSE CPP_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "kern/cpp_runtime/*.cpp")
file(GLOB_RECURSE LIBK_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "libk/*.c" "libk/*.cpp")
file(GLOB_RECURSE LIBK_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "libk/*.h" "libk/*.hpp")
file(GLOB_RECURSE LIBCPP_SOURCES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "external/libc++/*.cpp" "external/libc++/*.c")
file(GLOB_RECURSE LIBCPP_HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "external/libc++/*.hpp" "external/libc++/*.h")
file(GLOB_RECURSE HEADERS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "kern/*.hpp" "kern/*.h" "kern/*.tpp")
file(GLOB_RECURSE LD_SCRIPTS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "*.ld")

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/kern")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/kern/external")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/libk")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/external/libc++")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/external/acpica")

set(GENERIC_SRCS ${KERN_TERMINAL_SOURCES} ${CPP_SOURCES} ${KERN_BIOS_SOURCES} ${KERN_MB_SOURCES} ${KERN_BASE_SOURCES})

add_executable(${PROJECT_NAME} ${PLATFORM_SRCS} ${ISA_SRCS} ${GENERIC_SRCS} ${HEADERS} ${LD_SCRIPTS})

set(CMAKE_C_FLAGS "${ISA_C_FLAGS} ${PLATFORM_C_FLAGS} -O0 -fstack-protector -m32 -std=c11 -ftree-vectorize -Wall -Wextra -Wcast-qual -Wmisleading-indentation -Wno-unused-parameter -Wno-implicit-int -Wnull-dereference -lgcc -static-libgcc -ffreestanding -fno-builtin -nostdlib -finline-functions -fno-stack-protector -fomit-frame-pointer -Werror=implicit-function-declaration -mno-sse -lgcc")
set(CMAKE_CXX_FLAGS "${ISA_CXX_FLAGS} ${PLATFORM_CXX_FLAGS} -O0 -m32 -fstack-protector -std=c++17 -ftree-vectorize -Wall -Wold-style-cast -Wno-unused-parameter -Wextra -Wold-style-cast -Wcast-qual -Wmisleading-indentation -Wnull-dereference -lgcc -static-libgcc -ffreestanding -fno-builtin -nostdlib -finline-functions -fno-stack-protector -fomit-frame-pointer -Werror=implicit-function-declaration -mno-sse -fno-exceptions -fno-rtti -lgcc")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS
  "-T ${PLATFORM_LAYOUT} -N ${ISA_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS} -lgcc -static-libgcc -Xlinker -Map=ld.map")
set_target_properties(${PROJECT_NAME} PROPERTIES SUFFIX ".bin")
set(CMAKE_ASM_NASM_LINK_EXECUTABLE "${ARCH}-elf-gcc -T ${PLATFORM_LAYOUT} -N ${ISA_LINKER_FLAGS} ${PLATFORM_LINKER_FLAGS} <OBJECTS> -o <TARGET>.bin")

add_library(libk ${LIBK_SOURCES} ${LIBK_HEADERS})
target_compile_definitions(libk
    PUBLIC __is_libk)

add_library(libcpp ${LIBCPP_SOURCES} ${LIBCPP_HEADERS})

add_library(acpica ${ACPICA_SOURCES} ${ACPICA_HEADERS})
set_target_properties(acpica PROPERTIES LINKER_LANGUAGE C)
target_link_libraries(acpica libk)

include(ExternalProject)

target_link_libraries(${PROJECT_NAME} libk libcpp acpica gcc)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND "./multiboot-check.sh" $<TARGET_FILE:${PROJECT_NAME}>
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/tools")
