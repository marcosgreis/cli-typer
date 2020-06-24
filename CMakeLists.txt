cmake_minimum_required(VERSION 3.7)

project(cli-typer)

# ##############################################################################
# Flags
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -Wall -Werror -fstrict-aliasing -pedantic-errors -pedantic"
    )

# ##############################################################################
# boost
find_package(Boost)
include_directories(${Boost_INCLUDE_DIR})

# ##############################################################################
# clang-tidy
find_program(
    CLANG_TIDY_EXE
    NAMES "clang-tidy"
    DOC "Path to clang-tidy executable"
    )
if(NOT CLANG_TIDY_EXE)
    message(STATUS "clang-tidy not found.")
else()
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
    set(DO_CLANG_TIDY
        "${CLANG_TIDY_EXE}"
        "-checks=*,-fuchsia*,-clang-analyzer-alpha.*,-cppcoreguidelines-owning-memory,-modernize-use-trailing-return-type"
        )
endif()

# ##############################################################################

add_executable(
    cli-typer
    src/main.cpp
    src/Actions.hpp
    src/Actions.cpp
    src/Print.hpp
    src/Print.cpp
    src/Stream.hpp
    src/Stream.cpp
    )
