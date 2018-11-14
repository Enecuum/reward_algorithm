# Automatically set project name to directory name (if not set by user) and version based on git tags.

if (NOT DEFINED PROJECT)
    set (__PROJECT_NAME ${CMAKE_SOURCE_DIR})
    string (REGEX REPLACE ".*/" "" _PROJECT_NAME ${__PROJECT_NAME})
    # set (PROJECT_NAME ${_PROJECT_NAME} CACHE INTERNAL "")

    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git/config")
        execute_process (
            COMMAND git -C ${CMAKE_CURRENT_SOURCE_DIR} tag -l
            COMMAND sort -V
            COMMAND tail -n 1
            COMMAND sed -r "s,^v,,"
            COMMAND sed -r "s/[^0-9.].*//"
            COMMAND tr -d "\n"
            OUTPUT_VARIABLE GIT_LAST_VERSION
        )

        if ("x${GIT_LAST_VERSION}" STREQUAL "x")
            set (GIT_LAST_VERSION 0.0.0)
        endif ()
    else ()
        set (GIT_LAST_VERSION 0.0.0)
    endif ()

    project (${_PROJECT_NAME} VERSION ${GIT_LAST_VERSION})
endif()
