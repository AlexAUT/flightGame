cmake_minimum_required(VERSION 3.6)

PROJECT(flightGame)

#set(CMAKE_CXX_STANDARD 17)

set(SOURCE_FILES)
set(HEADER_FILES)

add_subdirectory(source)

if(${CMAKE_SYSTEM_NAME} MATCHES "Android")
    add_library(native-lib SHARED ${SOURCE_FILES})
    add_subdirectory(dependencies/awRevolt)
    set(THIRD_PARTY_DIR "${CMAKE_ANDROID_NDK}/sources/third_party")
    set(SFML_DIR "${THIRD_PARTY_DIR}/sfml/lib/${ANDROID_ABI}/cmake/SFML")
    find_package(SFML 2 COMPONENTS main system REQUIRED)
    target_link_libraries( 
                       native-lib

                       log
                       android
                       EGL
                       GLESv2

                       -Wl,--whole-archive sfml-main -Wl,--no-whole-archive

                       awEngine
                        ) 
else()
    add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${TEST_FILES})

    find_package(OpenGL)
    add_subdirectory(dependencies/awRevolt)
    target_link_libraries(${PROJECT_NAME} awEngine ${OPENGL_gl_LIBRARY})
endif()

#dependencies



