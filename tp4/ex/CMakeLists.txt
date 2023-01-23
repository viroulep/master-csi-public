# Pass -DZ3_LIBRARIES=/path/to/z3/bin and -DZ3_INCLUDE_DIR=/path/to/z3/include
# to help cmake or override the found library.
# With the default install Z3 should come with LLVM.
set(Z3_INCLUDE_DIR "${LLVM_INCLUDE_DIRS}" CACHE STRING "Path to Z3 include")
set(Z3_LIBRARIES "${LLVM_TOOLS_BINARY_DIR}" CACHE STRING "Path to Z3 lib")
find_package(Z3 4.8 REQUIRED)
message("Z3_INCLUDE_DIR:${Z3_INCLUDE_DIR}")
message("Z3_LIBRARIES:${Z3_LIBRARIES}")

set(TPZ3_SOURCES
  Function2Z3.cpp
  Register.cpp
  )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_llvm_pass_plugin(
  Z3Plugin
  ${TPZ3_SOURCES}
  SHARED
  LINK_LIBS z3)

target_include_directories(Z3Plugin
  PRIVATE
  ${CMAKE_CURRENT_BINARY_DIR}
  ${Z3_INCLUDE_DIR}
  )
target_link_directories(Z3Plugin
  PRIVATE
  ${Z3_LIBRARIES}
  )
target_link_options(Z3Plugin PRIVATE "-Wl,-rpath=${Z3_LIBRARIES}")

add_subdirectory(test)
