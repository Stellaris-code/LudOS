file(GLOB_RECURSE source_files "*.cpp")
list(REMOVE_ITEM source_files "main.cpp")
file(GLOB_RECURSE header_files "../include/*.hpp" "../include/*.tpp")

add_library(${PROJECT_NAME} ${header_files} ${source_files})

if (USE_CLANG_TIDY)
    set(DO_CLANG_TIDY "clang-tidy" "-checks=-*,clang-analyzer-*,-clang-analyzer-cplusplus*,cppcoreguidelines-*,llvm-include-order")
    set_target_properties(
      ${PROJECT_NAME} PROPERTIES
      CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
    )
endif()
