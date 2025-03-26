cmake_minimum_required(VERSION 3.10)

project(pheonix)

# 检查并设置C++标准
set(CMAKE_CXX_STANDARD 17)  # 设置C++17为最低标准
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # 强制至少使用C++17
set(CMAKE_CXX_EXTENSIONS OFF)  # 关闭编译器特定扩展，使用标准C++

# 如果可用，尝试使用更高版本的C++标准
include(CheckCXXCompilerFlag)

check_cxx_compiler_flag("-std=c++2a" COMPILER_SUPPORTS_CXX20)
if(COMPILER_SUPPORTS_CXX20)
    set(CMAKE_CXX_STANDARD 20)
elseif(COMPILER_SUPPORTS_CXX17)
    set(CMAKE_CXX_STANDARD 17)
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++17 support. Please use a different C++ compiler.")
endif()

set (3rdparty_DIRS ${PROJECT_SOURCE_DIR}/3rdparty)

file (GLOB_RECURSE CMAKE_FILES ${PROJECT_SOURCE_DIR}/src/CMakeLists.txt)
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