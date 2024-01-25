cmake_minimum_required(VERSION 3.10)

project(pheonix)

file (GLOB_RECURSE CMAKE_FILES ${PROJECT_SOURCE_DIR}/CMakeLists.txt)
foreach(CMAKE_FILE ${CMAKE_FILES})
    string(REGEX REPLACE "[/A-Za-z0-9_/]+/phoenix/" "" MODULE_NAME ${CMAKE_FILE})
    string(REGEX REPLACE "/CMakeLists.txt" "" MODULE_NAME ${MODULE_NAME})
    if (NOT (${MODULE_NAME} MATCHES "CMakeLists.txt"))
        list(APPEND MODULE_NAMES ${MODULE_NAME})
    endif()
endforeach()

foreach(MODULE_NAME ${MODULE_NAMES})
    add_subdirectory(${MODULE_NAME})
endforeach()

enable_testing()