cmake_minimum_required(VERSION 3.10)

project(sdk VERSION 0.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -g -O0")

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

add_library(DiscordSDK SHARED IMPORTED)

set_target_properties(DiscordSDK PROPERTIES
    IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/lib/x86_64/discord_game_sdk.so"
)

add_library(sdk SHARED sdk.c)

configure_file(sdk.pc.in sdk.pc @ONLY)

target_include_directories(sdk PUBLIC 
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/include"
    )

set_target_properties(sdk PROPERTIES 
    POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS})

target_link_libraries(sdk PUBLIC DiscordSDK)

