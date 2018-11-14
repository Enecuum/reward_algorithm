add_custom_target (
    included
    COMMAND ${CMAKE_CXX_COMPILER} -M ${LIB_HEADERS}
        \| tr ' ' '\\n'
        \| LC_COLLATE=C sort -f
        \| uniq
        # \| egrep -v '^ ?/usr/\(lib|include\)/'
        \| egrep -i '\(/[^/]+|\\.\(c|cpp|cxx|h|hpp|hxx\)$$\)'
        \> "${CMAKE_CURRENT_BINARY_DIR}/included.list"

    COMMAND echo -e "List of files included on ${ColourBold}this system${ColourReset}: ${ColourBold}${CMAKE_CURRENT_BINARY_DIR}/included.list${ColourReset}"
    BYPRODUCTS "${CMAKE_CURRENT_BINARY_DIR}/included.list"
    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
)
# message(STATUS "List of files included on ${ColourBold}this system${ColourReset} written to:\n\t${CMAKE_CURRENT_BINARY_DIR}/included.list")
