FUNCTION(ADD_TEST_PROGRAM NAME DIR)

    file(GLOB_RECURSE ${NAME}_SRCS RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${DIR}/*.cpp" "${DIR}/*.hpp")

    add_executable(${NAME} ${COMMON_SRCS} ${${NAME}_SRCS})
    set_target_properties(${NAME} PROPERTIES LINK_FLAGS
      "-T ${CMAKE_CURRENT_SOURCE_DIR}/layout.ld  -Xlinker")

  target_link_libraries(${NAME} libc)

    add_custom_command(TARGET ${NAME} POST_BUILD
      COMMAND "sed" "-i" "'1s/^/LUDOSBIN/'" $<TARGET_FILE:${NAME}>
      WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/tools")
ENDFUNCTION(ADD_TEST_PROGRAM)
