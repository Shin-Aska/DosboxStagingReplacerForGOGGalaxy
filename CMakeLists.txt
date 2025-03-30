cmake_minimum_required(VERSION 3.30)
project(DosboxStagingReplacer)

set(CMAKE_CXX_STANDARD 20)


# Add include directories for headers
include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}/libs/sqlite
)

add_executable(DosboxStagingReplacer
        libs/sqlite/sqlite3.c
        main.cpp
        helpers/scanners/DirectoryScanner.cpp
        helpers/scanners/DirectoryScanner.h
        helpers/finders/InstallationFinder.cpp
        helpers/finders/InstallationFinder.h
        helpers/verifiers/InstallationVerifier.cpp
        helpers/verifiers/InstallationVerifier.h
        factories/StatementEngineParserFactory.cpp
        factories/StatementEngineParserFactory.h
        interfaces/StatementParser.cpp
        interfaces/StatementParser.h
        models/Data.cpp
        models/Data.h
        services/sql/Service.cpp
        services/sql/Service.h
)
