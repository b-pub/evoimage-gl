cmake_minimum_required(VERSION 2.8)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/CMakeModules")

project(evoimage-gl)

# Locate necessary libraries
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)
find_package(PNG REQUIRED)
find_package(Cairo REQUIRED)

set(EPICFAILMSG "")
if(NOT OPENGL_FOUND)
    set(EPICFAILMSG "${EPICFAILMSG}  OpenGL not found\n")
endif()
if(NOT GLUT_FOUND)
    set(EPICFAILMSG "${EPICFAILMSG}  GLUT not found\n")
endif()
if(NOT PNG_FOUND)
    set(EPICFAILMSG "${EPICFAILMSG}  PNG not found\n")
endif()
if(NOT CAIRO_FOUND)
    set(EPICFAILMSG "${EPICFAILMSG}  Cairo not found\n")
endif()
if(EPICFAILMSG)
    message(FATAL_ERROR "Evoimage-gl needs the following libraries installed:\n${EPICFAILMSG}")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")

# Define the core GA engine library
file(GLOB ENGINE_SRCS engine/*.cpp)
add_library(engine STATIC
    ${ENGINE_SRCS})

include_directories(inc ${PNG_INCLUDE_DIRS} ${CAIRO_INCLUDE_DIRS})

# ----------------------------------------------------------------
# Executables to build:

#
# evoimagegl is the primary image evolver. This command-line program
# renders current mutations in the OpenGL window.
#
#add_executable(evoimagegl
#    src/evoimagegl.cpp
#)
#target_link_libraries(evoimagegl
#    engine
#    ${OPENGL_LIBRARIES}
#    ${GLUT_LIBRARIES}
#    ${PNG_LIBRARIES}
#)

#
# test_mutation is a command-line program that exercises the mutation engine
#
#add_executable(test_mutation
#    src/test_mutation.cpp
#)
#target_link_libraries(test_mutation
#    engine
#)

#
# evoimagecairo is another tool that uses the Cairo graphics lib for rendering
#
add_executable(evoimagecairo
    src/evoimagecairo.cpp
)
target_link_libraries(evoimagecairo
    engine
    ${CAIRO_LIBRARIES}
    ${PNG_LIBRARIES}
)
