cmake_minimum_required(VERSION 3.30)
project(DosboxStagingReplacer)

set(CMAKE_CXX_STANDARD 26)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/libs/sqlite)

add_executable(DosboxStagingReplacer main.cpp libs/sqlite/sqlite3.c)
