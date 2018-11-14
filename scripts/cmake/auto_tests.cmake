# Locate simple single C++ file tests in `./test/` and combine them in a
# `make check` target.

if (EXISTS "${CMAKE_SOURCE_DIR}/test")
    enable_testing()
    # Find tests
    execute_process (
        COMMAND find -L "${CMAKE_SOURCE_DIR}/test/" -mindepth 1 -maxdepth 1 -type f -regex ".*\\.\\(c\\|cpp\\|cxx|c\\+\\+\\)$"
        COMMAND sed -r "s|${CMAKE_SOURCE_DIR}/test/||"
        COMMAND sort
        COMMAND uniq
        COMMAND tr '\n' '\;'
        OUTPUT_VARIABLE TEST_SOURCES
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    # message(STATUS "Tests: \n${TEST_SOURCES}")

    set (TEST_LIST)
    set (TEST_NAMES)
    foreach (TEST_SOURCE IN LISTS TEST_SOURCES)
        if (NOT "_" STREQUAL "_${TEST_SOURCE}" AND EXISTS "${CMAKE_SOURCE_DIR}/test/${TEST_SOURCE}")
            string(REGEX REPLACE "\\.[^.]+$" "" TEST_NAME "${TEST_SOURCE}")
            list (APPEND TEST_NAMES ${TEST_NAME})
            # message(STATUS "Adding test ${ColourBold}${TEST_NAME}${ColourReset}")
            add_executable(test_${TEST_NAME} "${CMAKE_SOURCE_DIR}/test/${TEST_SOURCE}")

            set_target_properties(
                test_${TEST_NAME} PROPERTIES
                CXX_STANDARD 17
                CXX_STANDARD_REQUIRED ON
            )

            # Set any test compilation options here
            if ("_${CMAKE_BUILD_TYPE}" STREQUAL "_Release")
                target_compile_options(test_${TEST_NAME} PRIVATE "-std=gnu++17;-O3;-Wfatal-errors")
            else ()
                target_compile_options(test_${TEST_NAME} PRIVATE "-std=gnu++17;-O3;-ggdb;-Wall;-Wextra;-Wfatal-errors")
                if (CLANG_TIDY_EXE)
                    set_target_properties(
                        test_${TEST_NAME} PROPERTIES
                        CXX_CLANG_TIDY "${DO_CLANG_TIDY}"
                    )
                endif()
            endif ()

            add_test(${TEST_NAME} "test_${TEST_NAME}")
            list (APPEND TEST_LIST test_${TEST_NAME})
        endif()
    endforeach ()

    string (REGEX REPLACE "((^|;)[ \t]*([^;]+)($|;))" "\n\t\\3" TEST_NAMES "${TEST_NAMES}")
    message(STATUS "Available tests:${ColourBold}${TEST_NAMES}${ColourReset}")

    add_custom_target(check
        COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
        DEPENDS ${TEST_LIST}
    )
endif()
