cmake_minimum_required(VERSION 3.10)

project(fase3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

cmake_policy(SET CMP0072 NEW)

include_directories(utils)

add_executable(engine
    engine.cpp
    tinyXML/tinyxml2.cpp
    utils/Figure.cpp
    utils/Config.cpp
    utils/Group.cpp
)

add_executable(generator
    generator.cpp
    utils/Figure.cpp
)

if(WIN32)

    message(STATUS "Toolkits_DIR set to: ${TOOLKITS_FOLDER}")
    set(TOOLKITS_FOLDER "" CACHE PATH "Path to Toolkits folder")

    if(NOT EXISTS "${TOOLKITS_FOLDER}/glut/GL/glut.h" OR NOT EXISTS "${TOOLKITS_FOLDER}/glut/glut32.lib")
        message(FATAL_ERROR "GLUT not found")
    endif()

    include_directories(${TOOLKITS_FOLDER}/glut)

    find_package(OpenGL REQUIRED)

    target_link_libraries(engine
        ${OPENGL_LIBRARIES}
        ${TOOLKITS_FOLDER}/glut/glut32.lib
    )

    target_link_libraries(generator
        ${OPENGL_LIBRARIES}
        ${TOOLKITS_FOLDER}/glut/glut32.lib
    )

    if(EXISTS "${TOOLKITS_FOLDER}/glut/glut32.dll")
        file(COPY ${TOOLKITS_FOLDER}/glut/glut32.dll DESTINATION ${CMAKE_BINARY_DIR})
    endif()

    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT engine)

else()

    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-deprecated")

    find_package(OpenGL REQUIRED)
    find_package(GLUT REQUIRED)
    find_package(GLEW REQUIRED)

    include_directories(${OPENGL_INCLUDE_DIRS} ${GLUT_INCLUDE_DIRS})

    target_link_libraries(engine
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARIES}
        GLEW::GLEW
    )

    target_link_libraries(generator
        ${OPENGL_LIBRARIES}
        ${GLUT_LIBRARIES}
        GLEW::GLEW
    )

endif()