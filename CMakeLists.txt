cmake_minimum_required(VERSION 3.30)
project(DosboxStagingReplacer)

set(CMAKE_CXX_STANDARD 20)


# Add include directories for headers
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/sqlite
        ${CMAKE_CURRENT_SOURCE_DIR}/helpers
        ${CMAKE_CURRENT_SOURCE_DIR}
)

add_executable(DosboxStagingReplacer
        libs/sqlite/sqlite3.c
        main.cpp
)
