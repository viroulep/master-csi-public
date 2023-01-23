set(PACKAGE_FILE "${PROJECT_BINARY_DIR}/package.tar.gz")

message(STATUS "Creating package with courses: ${ENABLE_COURSES}")

# Make sure we always recreate the package by making it depend on a non existent
# file.
add_custom_target(student_package ALL DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/idontexist)

add_custom_command(
  OUTPUT ${PACKAGE_FILE} ${CMAKE_CURRENT_BINARY_DIR}/idontexist
  COMMAND ${Python_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/package.py ${PROJECT_SOURCE_DIR} ${PROJECT_BINARY_DIR} ${PACKAGE_FILE} ${ENABLE_COURSES}
  DEPENDS package.py cut.py student_package
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  COMMENT "Packing courses...")
