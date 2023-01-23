cmake_minimum_required(VERSION 3.13)

project(CSIObfuscation)

if("${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
  message(FATAL_ERROR "Do not set the build directory equal to the source directory!")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/")


# NOTE: it's actually a very bad practice to hardcode a path in there,
# I'm just doing this so that it's as easy as "cmake .." to configure this
# project at CREMI.
set(DEPENDENCIES_ROOT "/net/ens/csi/llvm" CACHE STRING
  "Additional path to the installation of dependencies.")

find_package(LLVM 15 REQUIRED CONFIG HINTS ${DEPENDENCIES_ROOT})
message(STATUS "Found LLVM in ${LLVM_CMAKE_DIR}")

# LLVM-specific options, that will be used in all courses
add_compile_options(-fno-rtti)
include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})

find_package(Python 3 REQUIRED)

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(ALL_COURSES "tp1;tp2;tp3;tp4")
set(DEFAULT_COURSES "all")

set(ENABLE_COURSES "default" CACHE STRING
  "Semicolon-separated list of projects to build (${ALL_COURSES}), or \"all\".")

if(ENABLE_COURSES STREQUAL "default")
  set(ENABLE_COURSES ${DEFAULT_COURSES})
endif()
if(ENABLE_COURSES STREQUAL "all" )
  set(ENABLE_COURSES ${ALL_COURSES})
endif()

foreach(course ${ALL_COURSES})
  if ("${course}" IN_LIST ENABLE_COURSES)
    message(STATUS "${course} is enabled")
    add_subdirectory(${course})
  else()
    message(STATUS "${course} is disabled")
  endif()
endforeach()

add_subdirectory(package)
