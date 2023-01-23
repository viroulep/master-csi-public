set(OC_SOURCES
  OpaqueConstants.cpp
  AbstractFilterFactory.cpp
  MBAHandlers.cpp
  LiveValues.cpp
  HandleMBA.cpp
  HandleSoft.cpp
  HandleHard.cpp
  Register.cpp
  )

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_llvm_pass_plugin(
  OCPlugin
  ${OC_SOURCES}
  SHARED)

add_subdirectory(test)
