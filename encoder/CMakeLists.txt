cmake_minimum_required(VERSION 3.10)

project(encoder VERSION 0.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g -O0")

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

add_library(encoder SHARED encoder.c)

configure_file(encoder.pc.in encoder.pc @ONLY)

target_include_directories(encoder PUBLIC 
    "${PROJECT_BINARY_DIR}"
)

set_target_properties(encoder PROPERTIES 
    POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})
