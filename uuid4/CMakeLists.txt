cmake_minimum_required(VERSION 3.10)

project(uuid VERSION 0.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g -O0")

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

add_library(uuid SHARED uuid.c)

target_include_directories(uuid PUBLIC 
    "${PROJECT_BINARY_DIR}"
    )

set_target_properties(uuid PROPERTIES 
    POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})
