cmake_minimum_required(VERSION 3.6)

project(flightGame VERSION 0.0.1 DESCRIPTION "Teambased flight game based on awRevolt engine")

set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES)
set(HEADER_FILES)

add_subdirectory(source)

add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${TEST_FILES})

#dependencies

add_subdirectory(dependencies/awRevolt)
find_package(OpenGL)

target_link_libraries(${PROJECT_NAME} awEngine ${OPENGL_gl_LIBRARY})