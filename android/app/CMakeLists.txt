cmake_minimum_required(VERSION 3.4.1)

set(AW_ANDROID true)
set(AW_USE_GLES true)
add_definitions(-DAW_USE_GLES)
add_definitions(-DAW_ANDROID)


message(STATUS "SFML include dir: ${SFML_INCLUDE_DIR}")
message(STATUS "SFML lib dir: ${SFML_LIBRARY_DIR}${ANDROID_ABI}/")
message(STATUS "SFML extlib dir: ${SFML_EXT_LIBRARY_DIR}${ANDROID_ABI}/")
message(STATUS "AW revolt include dir: ${AW_REVOLT_INCLUDE_DIR}")
message(STATUS "AW lib dir: ${AW_REVOLT_LIBRARY_DIR}${ANDROID_ABI}/")
message(STATUS "AW ext inc dir: ${AW_REVOLT_EXT_INCLUDE_DIR}${ANDROID_ABI}/")
message(STATUS "AW ext lib dir: ${AW_REVOLT_EXT_LIBRARY_DIR}${ANDROID_ABI}/")
message(STATUS "ABI ${ANDROID_ABI}")

include_directories(${SFML_INCLUDE_DIR})
include_directories(${AW_REVOLT_INCLUDE_DIR})
include_directories(${AW_REVOLT_EXT_INCLUDE_DIR})

link_directories("${SFML_LIBRARY_DIR}${ANDROID_ABI}/")
link_directories("${SFML_EXT_LIBRARY_DIR}${ANDROID_ABI}/")
link_directories("${AW_REVOLT_LIBRARY_DIR}${ANDROID_ABI}/")
link_directories("${AW_REVOLT_EXT_LIBRARY_DIR}${ANDROID_ABI}/")



file(GLOB_RECURSE SOURCE_FILES ../../source/*.cpp)
message(STATUS "Source files: ${SOURCE_FILES}")

add_library( # Sets the name of the library.
             native-lib

             # Sets the library as a shared library.
             SHARED

             # Provides a relative path to your source file(s).
             #../../source/main.cpp
             ${SOURCE_FILES}
             )


# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.

                       native-lib

                       # Links the target library to the log library
                       # included in the NDK.

                       log
                       android
                       EGL
                       GLESv2

                       sfml-system

                       openal

                       sfml-window
                       sfml-graphics
                       sfml-activity
                       -Wl,--whole-archive sfml-main -Wl,--no-whole-archive

                       awRevolt
                       assimp
                       jsoncpp
                        )
