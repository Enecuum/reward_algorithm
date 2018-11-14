# Generate API documentation with Doxygen ==========================================================
# Authors:
# git shortlog -nse | sed -r 's/^[ \t\s]*[0-9]+[ \t\s]+//' | sort | uniq

find_package (Doxygen)
if (DOXYGEN_FOUND)
    add_custom_target (
        doc
        ${DOXYGEN_EXECUTABLE}
        ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen" VERBATIM
    )
    if (NOT EXISTS ${CMAKE_CURRENT_BINARY_DIR}/README.md)
        if (EXISTS ${CMAKE_SOURCE_DIR}/README.md)
            add_custom_command(
                TARGET doc
                POST_BUILD
                COMMAND ln -s ${CMAKE_SOURCE_DIR}/README.md ${CMAKE_CURRENT_BINARY_DIR}/README.md
            )
        else ()
            add_custom_command(
                TARGET doc
                POST_BUILD
                COMMAND echo \"\# ${PROJECT_NAME}\"         > ${CMAKE_CURRENT_BINARY_DIR}/README.md
                COMMAND echo \"\"                           >> ${CMAKE_CURRENT_BINARY_DIR}/README.md
                COMMAND echo \"${PROJECT_VERSION_STRING}\"  >> ${CMAKE_CURRENT_BINARY_DIR}/README.md
                COMMAND echo \"\"                           >> ${CMAKE_CURRENT_BINARY_DIR}/README.md
                COMMAND echo \"\#\# Changelog\"             >> ${CMAKE_CURRENT_BINARY_DIR}/README.md
                COMMAND echo \"\"                           >> ${CMAKE_CURRENT_BINARY_DIR}/README.md
                COMMAND echo \"Last 20 updates:\"           >> ${CMAKE_CURRENT_BINARY_DIR}/README.md
                COMMAND git log -20 --pretty='format:    * <span data-datetime='%cI'>%ad</span> <span class='commit-hash'>%H</span> <span class='commit-author'>%an</span> <pre class='commit-message'>%s</pre>' --date=short >> ${CMAKE_CURRENT_BINARY_DIR}/README.md
                COMMAND echo \"\"                           >> ${CMAKE_CURRENT_BINARY_DIR}/README.md
                COMMAND echo \"\"                           >> ${CMAKE_CURRENT_BINARY_DIR}/README.md
            )
        endif ()
    endif ()
    add_custom_command(
        TARGET doc
        POST_BUILD
        COMMAND cp ${CMAKE_CURRENT_BINARY_DIR}/README.md ${CMAKE_SOURCE_DIR}/doc/
    )
    configure_file (${CMAKE_SOURCE_DIR}/scripts/doxygen/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    if (NOT "_" STREQUAL "_${PROJECT_VERSION}")
        add_custom_command(
            TARGET doc
            POST_BUILD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/doc/latex
            COMMAND make
            COMMAND mv refman.pdf ../${PROJECT_NAME}_v${PROJECT_VERSION}.pdf
        )
    else()
        add_custom_command(
            TARGET doc
            POST_BUILD
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/doc/latex
            COMMAND make
            COMMAND mv refman.pdf ../${PROJECT_NAME}.pdf
        )
    endif()
    add_custom_target(docs)
    add_dependencies( docs doc)
endif(DOXYGEN_FOUND)
#===================================================================================================
