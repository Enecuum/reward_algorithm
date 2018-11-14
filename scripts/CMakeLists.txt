# Universal simple C++ header-only library script.
# This aims to automate as much as possible based on some assumptions:
# 1. The project name equals the base directory name
# 2. A Linux system with a pretty decent toolset is required.
# 3. Documentation will be extracted from source by Doxygen and compiled to HTML,
#    PDF and man pages, placed in `./doc/`
# 4. The standard `make install` target will use default configurations.
# 5. Each C++ source in `./test/` is considered a self-contained test and will
#    be added to the `make check` target.
# 6. The automatic project name script will attempt to extract the latest version
#    based on Git tags in the form of `v[0-9]+.[0-9]+.[0-9]+`, eg `v1.2.03`.
# 7. The headers comprising the library are placed in the project root.

cmake_minimum_required (VERSION 3.8)
set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/scripts/cmake)

# These two prevent building in the source directory. `mkdir ./build` instead.
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

include(colours)

# Project name - exported to source as PROJECT_NAME
# If you leave this commented out the base directory name is used as project
# name, with either the most recent git tag or '0.0.0' as version.
# project (my_library VERSION 1.0.0)
include(auto_project_name)

message(STATUS "Project name: ${ColourBold}${PROJECT_NAME}${ColourReset}")
message(STATUS "Project version: ${ColourBold}${PROJECT_VERSION}${ColourReset}")

# Build options.
# Hint: If you use `#ifndef NDEBUG` for debug code this will automatically toggle those code blocks.
if (NOT CMAKE_BUILD_TYPE)
    # set (CMAKE_BUILD_TYPE Release)
    set (CMAKE_BUILD_TYPE Debug)
endif ()

message (STATUS "Build type: ${ColourBold}${CMAKE_BUILD_TYPE}${ColourReset}")

add_custom_target(build-time-make-directory1
    PRE_BUILD ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_SOURCE_DIR}/doc"
)

add_library(
    ${PROJECT_NAME}
    INTERFACE
)

# Find local headers
execute_process (
    COMMAND find -L "${CMAKE_SOURCE_DIR}/" -mindepth 1 -maxdepth 1 -type f -regex ".*\\.\\(h\\|hpp\\|hxx\\)$"
    COMMAND sed -r "s|${CMAKE_SOURCE_DIR}/||"
    COMMAND sort
    COMMAND uniq
    COMMAND tr '\n' '\;'
    OUTPUT_VARIABLE LIB_HEADERS
    OUTPUT_STRIP_TRAILING_WHITESPACE
)
string (REGEX REPLACE "((^|;)([^;]+)(;|$))" "\n\t\\3" LIB_HEADERS_PRETTY "${LIB_HEADERS}")
message(STATUS "Library headers: ${ColourBold}${LIB_HEADERS_PRETTY}${ColourReset}")

# Install headers found above
install(FILES
    ${LIB_HEADERS}
    DESTINATION
    include/${PROJECT_NAME}
)

# Comment this out to disable running clang-tidy on test executables in debug mode.
# For a full list of tidy checks, with default enabled ones marked (*), run this very ugly bash mashup:
# { clang-tidy -list-checks -checks=*; clang-tidy -list-checks; } | egrep -v '^([a-zA-Z]|$)' | sort | uniq -c | sed -r 's,\s+1\s+,\t\t,' | sed -r 's,\s+[2-9]\s+,\t*\t,'
# Currently clang-tidy seems to complain about a missing compilation database, breaking debug builds
# find_program(
#     CLANG_TIDY_EXE
#     NAMES "clang-tidy"
#     DOC "Path to clang-tidy executable"
# )
if(CLANG_TIDY_EXE)
    message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
    set(DO_CLANG_TIDY "${CLANG_TIDY_EXE}" "-extra-arg=-fcolor-diagnostics;-extra-arg=-std=c++17")
endif()

include(auto_tests)

include(generate_documentation)

include (included_list)

#TODO: Sort out how much of this we really want to install:
# include(CMakePackageConfigHelpers)
# write_basic_package_version_file(
#   "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
#   VERSION ${PROJECT_VERSION}
#   COMPATIBILITY AnyNewerVersion
# )

# install(TARGETS
#     ${PROJECT_NAME}
#     EXPORT library_targets
#     RUNTIME DESTINATION bin
#     ARCHIVE DESTINATION lib
#     LIBRARY DESTINATION lib
#     INCLUDES DESTINATION include
# )

# export(EXPORT library_targets
#   FILE "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-targets.cmake"
#   NAMESPACE ${PROJECT_NAME}::
# )

# configure_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
#   "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
#   COPYONLY
# )

# set(ConfigPackageLocation lib/cmake/${PROJECT_NAME})
# install(EXPORT library_targets
#   FILE
#     ${PROJECT_NAME}-targets.cmake
#   NAMESPACE
#     ${PROJECT_NAME}::
#   DESTINATION
#     ${ConfigPackageLocation}
# )
# install(
#   FILES
#     "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
#   DESTINATION
#     ${ConfigPackageLocation}
#   COMPONENT
#     Devel
# )
