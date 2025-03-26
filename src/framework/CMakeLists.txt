cmake_minimum_required(VERSION 3.0.2)

project(core)

find_package(GTest REQUIRED)

set(COMMON_DIR ${PROJECT_SOURCE_DIR}/../common)
include_directories(
    ${COMMON_DIR}
    ${PROJECT_SOURCE_DIR}/../
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
    target_link_libraries(${T} PRIVATE GTest::GTest GTest::Main)
    gtest_discover_tests(${T})
endforeach()

enable_testing()