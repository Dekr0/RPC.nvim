cmake_minimum_required(VERSION 3.10)

project(Middleware VERSION 0.1)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

add_library(DiscordSDK SHARED IMPORTED)

set_target_properties(DiscordSDK PROPERTIES
    IMPORTED_LOCATION "${PROJECT_SOURCE_DIR}/lib/x86_64/discord_game_sdk.so"
)

configure_file(middleware_config.h.in middleware_config.h)

add_executable(Middleware middleware.c init.c logger.c) 

target_link_libraries(Middleware PUBLIC DiscordSDK)

target_include_directories(
    Middleware
    PUBLIC
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/include"
)

install(TARGETS Middleware DESTINATION 
    "$ENV{HOME}/.local/share/nvim/richpresence.nvim")
