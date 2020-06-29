cmake_minimum_required(VERSION 3.15)
project(MDF_BusLogging_PythonModule)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
set(VERSION_PATCH 0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/CMake")

# Search for Python.
find_package(Python3 COMPONENTS Development REQUIRED)
find_package(Threads)

find_package(PyCXX REQUIRED)

# List of all source files.
set(SOURCE_FILES
    Source/CallbackBuffer.cpp
    Source/CallbackBuffer.h
    Source/Export.cpp
    Source/MdfFileWrapper.cpp
    Source/MdfFileWrapper.h
    Source/MdfCANRecordWrapper.cpp
    Source/MdfCANRecordWrapper.h
    Source/MdfCANIteratorWrapper.cpp
    Source/MdfCANIteratorWrapper.h
    Source/PythonLogger.cpp
    Source/PythonLogger.h

    Source/Module.cpp
    Source/Module.h
    )

if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "4")
    set(PY3LIB "C:/Programs/Python/Python38-i386/libs/python3.lib")
else()
    set(PY3LIB "C:/Programs/Python/Python38-x86_64/libs/python3.lib")
endif()

# Create python extension.
set(LIBRARY_NAME mdfsimple)
add_library(${LIBRARY_NAME} SHARED ${SOURCE_FILES})
link_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${Python_INCLUDE_DIRS})
target_link_libraries(${LIBRARY_NAME}
    Threads::Threads
    PyCXX::PyCXX
    MDF_BusLogging_Library_static
    Python3::Module
    Python3::Python
    #${PY3LIB}
    )
#set_target_properties(${LIBRARY_NAME} PROPERTIES SUFFIX ".pyd")
message(Python3::Module)
message("${PyCXX_INCLUDE_DIRS}")
message("${PyCXX_SOURCES}")
set( CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--no-undefined" )
set_target_properties(${LIBRARY_NAME} PROPERTIES PREFIX "")
add_custom_command(TARGET ${LIBRARY_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${LIBRARY_NAME}> ${CMAKE_CURRENT_SOURCE_DIR}/Test
    )

#add_executable(mdfsimpledemo Source/main.cpp)
#target_link_libraries(mdfsimpledemo mdfsimple)
