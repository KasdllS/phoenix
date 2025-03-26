cmake_minimum_required(VERSION 3.10)

project(parser)

# nlohmann
set (JSON_DIR ${3rdparty_DIRS}/json/single_include/)

file (GLOB SOURCE_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
find_package(yaml-cpp REQUIRED)
find_package(GTest REQUIRED)

include_directories(
    ${JSON_DIR}
    ${PROJECT_SOURCE_DIR}/../
)

add_library(
    ${PROJECT_NAME}
    SHARED
    ${SOURCE_FILES}
)
target_link_libraries(
    ${PROJECT_NAME}
    PRIVATE
    yaml-cpp
)

# gtest单元测试
file(GLOB_RECURSE UNITEST_FILES  ${PROJECT_SOURCE_DIR}/*_unitest.cc)
foreach(TEST_FILE ${UNITEST_FILES})
    string(REGEX REPLACE ".+/(.+)\\..*" "\\1" MODULE_NAME ${TEST_FILE})
    add_executable(${PROJECT_NAME}.${MODULE_NAME} ${TEST_FILE})
    list(APPEND UNITEST_TARGETS ${PROJECT_NAME}.${MODULE_NAME})
endforeach()
include(GoogleTest)
foreach(T ${UNITEST_TARGETS})
    target_link_libraries(${T} PRIVATE GTest::GTest GTest::Main ${PROJECT_NAME})
    gtest_discover_tests(${T})
endforeach()

enable_testing()