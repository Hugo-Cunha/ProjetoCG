cmake_minimum_required(VERSION 3.5)

project(fase4)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Resolve o aviso do OpenGL em sistemas mais recentes
cmake_policy(SET CMP0072 NEW)

# Ajuda o CMake a encontrar bibliotecas instaladas pelo Homebrew no macOS
if(APPLE)
    list(APPEND CMAKE_PREFIX_PATH "/opt/homebrew" "/usr/local")
endif()

find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(GLEW REQUIRED)
find_package(DevIL REQUIRED)

include_directories(
    ${OPENGL_INCLUDE_DIRS}
    ${GLUT_INCLUDE_DIR}
    ${IL_INCLUDE_DIR}
    utils
)

add_executable(
    engine
    engine.cpp
    tinyXML/tinyxml2.cpp
    utils/Figure.cpp
    utils/Config.cpp
    utils/Group.cpp
)

add_executable(
    generator
    generator.cpp
    utils/Figure.cpp
)

if(WIN32)

    message(STATUS "Toolkits_DIR set to: ${TOOLKITS_FOLDER}")
    set(TOOLKITS_FOLDER "" CACHE PATH "Path to Toolkits folder")

    if(NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")
        message(ERROR ": GLUT not found")
    endif()

    include_directories(${TOOLKITS_FOLDER}/glut)

    target_link_libraries(
        engine
        ${OPENGL_LIBRARIES}
        ${TOOLKITS_FOLDER}/glut/glut32.lib
        GLEW::GLEW
        ${IL_LIBRARIES}
    )

    target_link_libraries(
        generator
        ${OPENGL_LIBRARIES}
        ${TOOLKITS_FOLDER}/glut/glut32.lib
        GLEW::GLEW
        ${IL_LIBRARIES}
    )

    if(EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll")
        file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll DESTINATION ${CMAKE_BINARY_DIR})
    endif()

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT engine)

else()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")

    target_link_libraries(
        engine
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARIES}
        GLEW::GLEW
        ${IL_LIBRARIES}
    )

    target_link_libraries(
        generator
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARIES}
        GLEW::GLEW
        ${IL_LIBRARIES}
    )

endif()