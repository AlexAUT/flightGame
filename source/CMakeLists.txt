add_subdirectory(states)
add_subdirectory(game)
add_subdirectory(opengl)

set(HEADER_FILES
  ${HEADER_FILES}
  PARENT_SCOPE
)

set(SOURCE_FILES
  ${SOURCE_FILES}
  ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
  PARENT_SCOPE
)

