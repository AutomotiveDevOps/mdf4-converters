# See https://cmake.org/cmake/help/latest/module/FindPackageHandleStandardArgs.htm
# See https://cmake.org/cmake/help/latest/manual/cmake-developer.7.html

# Since the default FindPython seems to always select the highest available, and misses a target for the major-only
# library, this custom file is used instead.

if (NOT python_ROOT)
    set(python_ROOT "$ENV{python_ROOT}")
endif ()

if (NOT python_ROOT)
    find_path(_python_ROOT NAMES include/Python.h)
else ()
    set(_python_ROOT ${python_ROOT})
endif ()

find_path(PythonMulti_INCLUDE_DIR
    NAMES Python.h
    HINTS
    ${_python_ROOT}/include
    ${_python_ROOT}/include/python3.7
    ${_python_ROOT}/include/python3.7m
    ${_python_ROOT}/include/python3.8
    ${_python_ROOT}/include/python3.8m
    )

if (NOT ${PythonMulti_INCLUDE_DIR} STREQUAL "python_INCLUDE_DIR-NOTFOUND")
    # Path to common file with configuration parameters.
    set(_python_H ${PythonMulti_INCLUDE_DIR}/Python.h)
    set(_pathlevel_H ${PythonMulti_INCLUDE_DIR}/patchlevel.h)

    # Function to extract version information from the header.
    function(_python_EXTRACT _python_VER_MAJOR_OUTPUT _python_VER_MINOR_OUTPUT _python_VER_PATCH_OUTPUT)
        set(CMAKE_MATCH_1 "0")
        set(CMAKE_MATCH_2 "0")
        set(CMAKE_MATCH_3 "0")
        set(_python_expr "^[ \\t]*#define[ \\t]+PY_VERSION[ \\t]+\"([0-9]).([0-9]+).([0-9]+)\"")
        file(STRINGS "${_pathlevel_H}" _python_ver REGEX "${_python_expr}")
        string(REGEX MATCH "${_python_expr}" python_ver "${_python_ver}")
        string(REPLACE "0" "" _match_1 ${CMAKE_MATCH_1})
        string(REPLACE "0" "" _match_2 ${CMAKE_MATCH_2})
        string(REPLACE "0" "" _match_3 ${CMAKE_MATCH_3})
        set(${_python_VER_MAJOR_OUTPUT} "${_match_1}" PARENT_SCOPE)
        set(${_python_VER_MINOR_OUTPUT} "${_match_2}" PARENT_SCOPE)
        set(${_python_VER_PATCH_OUTPUT} "${_match_3}" PARENT_SCOPE)
    endfunction()

    # Extract total version information.
    _python_EXTRACT(PYTHON_VERSION_MAJOR PYTHON_VERSION_MINOR PYTHON_VERSION_PATCH)

    # Split into major, minor and patch.


    if (python_FIND_VERSION_COUNT GREATER 2)
        set(PythonMulti_VERSION "${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}.${PYTHON_VERSION_PATCH}")
    else ()
        set(PythonMulti_VERSION "${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}")
    endif ()

    find_library(PythonMulti_LIBRARY
        NAMES
        "python${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}"
        "python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}"
        "python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}m"
        "libpython${PYTHON_VERSION_MAJOR}${PYTHON_VERSION_MINOR}"
        "libpython${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}"
        "libpython${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}m"
        HINTS
        ${_python_ROOT}/lib
        ${_python_ROOT}/libs
        )
    find_library(PythonMulti_generic_LIBRARY
        NAMES
        python3
        libpython3
        HINTS
        ${_python_ROOT}/lib
        ${_python_ROOT}/libs
        )
    find_program(PythonMulti_INTERPRETER
        NAMES
        python
        python3
        HINTS
        ${_python_ROOT}
        ${_python_ROOT}/bin
        )

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(PythonMulti
        FOUND_VAR
        PythonMulti_FOUND
        REQUIRED_VARS
        PythonMulti_INCLUDE_DIR
        PythonMulti_LIBRARY
        PythonMulti_generic_LIBRARY
        PythonMulti_INTERPRETER
        VERSION_VAR
        PythonMulti_VERSION
        )

    if (PythonMulti_FOUND)
        # Export variables.
        set(PythonMulti_LIBRARIES ${PythonMulti_LIBRARY})
        set(PythonMulti_INCLUDE_DIRS ${PythonMulti_INCLUDE_DIR})

        # Create target if not already present.
        if (NOT TARGET PythonMulti::Module)
            add_library(PythonMulti::Module UNKNOWN IMPORTED)
            set_target_properties(PythonMulti::Module PROPERTIES
                IMPORTED_LOCATION "${PythonMulti_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${PythonMulti_INCLUDE_DIR}"
                )
        endif ()
        if (NOT TARGET PythonMulti::Generic)
            add_library(PythonMulti::Generic UNKNOWN IMPORTED)
            set_target_properties(PythonMulti::Generic PROPERTIES
                IMPORTED_LOCATION "${PythonMulti_generic_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${PythonMulti_INCLUDE_DIR}"
                )
        endif ()
        if (NOT TARGET PythonMulti::Interpreter)
            add_executable(PythonMulti::Interpreter IMPORTED)
            set_target_properties(PythonMulti::Interpreter PROPERTIES
                IMPORTED_LOCATION "${PythonMulti_INTERPRETER}"
                )
        endif ()
        message("Found generic python ${PythonMulti_VERSION}")
    endif ()
endif ()
