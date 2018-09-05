set(HEADER_FILES
  ${HEADER_FILES}
  ${CMAKE_CURRENT_SOURCE_DIR}/airplane.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/airplaneController.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cameraController.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/level.hpp
  ${CMAKE_CURRENT_SOURCE_DIR}/resourceManager.hpp
  PARENT_SCOPE
)

set(SOURCE_FILES
  ${SOURCE_FILES}
  ${CMAKE_CURRENT_SOURCE_DIR}/airplane.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/airplaneController.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/cameraController.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/level.cpp
  ${CMAKE_CURRENT_SOURCE_DIR}/resourceManager.cpp
  PARENT_SCOPE
)

