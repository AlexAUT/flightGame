cmake_minimum_required(VERSION 3.6)

project(flightGame VERSION 0.0.1 DESCRIPTION "Teambased flight game based on awRevolt engine")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS "ON")

if(${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
  set(AW_EMSCRIPTEN true)
  set(AW_USE_GLES true)
  add_definitions(-DAW_USE_GLES)
  add_definitions(-DAW_EMSCRIPTEN)
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Android")
  set(AW_ANDROID true)
  set(AW_USE_GLES true)
  add_definitions(-DAW_USE_GLES)
  add_definitions(-DAW_ANDROID)
else()
  set(AW_DESKTOP true)
  set(AW_USE_OPENGL true)
  add_definitions(-DAW_DESKTOP)
  add_definitions(-DAW_USE_OPENGL)
endif()


set(SOURCE_FILES)
set(HEADER_FILES)

add_subdirectory(source)

if(AW_DESKTOP)

    add_executable(${PROJECT_NAME} ${SOURCE_FILES} ${HEADER_FILES} ${TEST_FILES})

    # Link everyhing
    # Find SFML
    #set(SFML_DIR "${CMAKE_SOURCE_DIR}/extlibs/lib/cmake/SFML")
    #find_package(SFML 2 REQUIRED  window system)
    target_link_libraries(${PROJECT_NAME} sfml-window sfml-system)

    # Setup assimp, we need to create a target ourself
    FILE(GLOB ASSIMP_DIR "${CMAKE_SOURCE_DIR}/extlibs/lib/cmake/assimp*")
    target_link_libraries(${PROJECT_NAME} ${ASSIMP_LIBRARY})
    #jsoncpp
    set(jsoncpp_DIR "${CMAKE_SOURCE_DIR}/extlibs/lib64/cmake/jsoncpp")
    find_package(jsoncpp REQUIRED)
    target_link_libraries(${PROJECT_NAME} jsoncpp)

    target_link_libraries(${PROJECT_NAME} awRevolt GL)

elseif(AW_ANDROID)
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

else()
    message(ERROR "Platform not supported!")
endif()