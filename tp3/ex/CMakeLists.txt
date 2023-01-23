configure_file(clang_config.h.in ${CMAKE_CURRENT_BINARY_DIR}/clang_config.h)

set(BUILTIN_FILE ${CMAKE_CURRENT_BINARY_DIR}/builtins.h)
add_custom_command(OUTPUT ${BUILTIN_FILE}
  COMMAND ${Python_EXECUTABLE} ./builtins.py builtin/readlink_gdb.c builtin/detect_ptrace.c > ${BUILTIN_FILE}
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS builtins.py builtin/detect_ptrace.c builtin/readlink_gdb.c
  COMMENT "Generating builtin header"
)
add_custom_target(course3-gen-builtin DEPENDS ${BUILTIN_FILE})

include_directories(${CMAKE_CURRENT_BINARY_DIR} ${CMAKE_CURRENT_SOURCE_DIR})

set(AD_SOURCES
  AntiDebug.cpp
  InjectBuiltin.cpp
  Injectors.cpp
  Register.cpp
  )

add_llvm_pass_plugin(
  ADPlugin
  ${AD_SOURCES}
  SHARED
  DEPENDS
  course3-gen-builtin)

add_subdirectory(test)
