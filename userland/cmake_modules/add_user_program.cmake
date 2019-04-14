FUNCTION(ADD_TEST_PROGRAM NAME DIR)

    file(GLOB_RECURSE ${NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${DIR}/*.cpp" "${DIR}/*.c" "${DIR}/*.h" "${DIR}/*.hpp" "${DIR}/*.def")

    add_executable(${NAME} ${COMMON_SRCS} ${${NAME}_SRCS})
    set_target_properties(${NAME} PROPERTIES LINK_FLAGS
      "-T ${CMAKE_CURRENT_SOURCE_DIR}/layout.ld  -Xlinker")

  include_directories("${CMAKE_SOURCE_DIR}/userland/external/pthread-embedded")
  include_directories("${CMAKE_SOURCE_DIR}/userland/external/pthread-embedded/platform/ludos")
  target_link_libraries(${NAME} libc user_libcpp pte)
  target_compile_definitions(${NAME} PUBLIC "-DLUDOS_USER")

#    add_custom_command(TARGET ${NAME} POST_BUILD
#      COMMAND "sed" "-i" "'1s/^/LUDOSBIN/'" $<TARGET_FILE:${NAME}>
#      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/tools")
ENDFUNCTION(ADD_TEST_PROGRAM)

FUNCTION(ADD_USER_PROGRAM NAME DIR)

    file(GLOB_RECURSE ${NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${DIR}/*.cpp" "${DIR}/*.c" "${DIR}/*.h" "${DIR}/*.hpp" "${DIR}/*.def")

    add_executable(${NAME} ${COMMON_SRCS} ${${NAME}_SRCS})
    set_target_properties(${NAME} PROPERTIES LINK_FLAGS
      "-T ${CMAKE_CURRENT_SOURCE_DIR}/layout.ld  -Xlinker")

  include_directories("${CMAKE_SOURCE_DIR}/userland/external/pthread-embedded")
  include_directories("${CMAKE_SOURCE_DIR}/userland/external/pthread-embedded/platform/ludos")
  target_link_libraries(${NAME} libc user_libcpp pte)
  target_compile_definitions(${NAME} PUBLIC "-DLUDOS_USER")

#    add_custom_command(TARGET ${NAME} POST_BUILD
#      COMMAND "sed" "-i" "'1s/^/LUDOSBIN/'" $<TARGET_FILE:${NAME}>
#      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/tools")
ENDFUNCTION(ADD_USER_PROGRAM)

