cmake_minimum_required(VERSION 3.10)

project(ipcwriter VERSION 0.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Werror -g -O0")

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

add_subdirectory(nanopb)

add_library(ipcwriter SHARED ipcwriter.c schema.pb.c)

configure_file(ipcwriter.pc.in ipcwriter.pc @ONLY)

target_include_directories(ipcwriter PUBLIC 
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/nanopb"
    )

set_target_properties(ipcwriter PROPERTIES 
    POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})

target_link_libraries(ipcwriter PUBLIC protobuf-nanopb)

