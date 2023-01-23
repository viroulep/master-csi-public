set(MBA_SOURCES
  MBA.cpp
  MBAHandlers.cpp
  Register.cpp
  )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_llvm_pass_plugin(
  MBAPlugin
  ${MBA_SOURCES}
  SHARED)

add_subdirectory(test)
