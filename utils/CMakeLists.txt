cmake_minimum_required(VERSION 3.10)

project(phoenix_utils)

file(GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/*.cpp)
add_library(
    ${PROJECT_NAME}
    SHARED
    ${SOURCE_FILES}
    )
target_include_directories(
    ${PROJECT_NAME}
    PRIVATE
    ${PROJECT_SOURCE_DIR}/../
    )

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_EXTENSIONS OFF)

find_package(GTest REQUIRED)

file(GLOB_RECURSE TEST_FILES  ${PROJECT_SOURCE_DIR}/*_test.cc)

foreach(TEST_FILE ${TEST_FILES})
    string(REGEX REPLACE ".+/(.+)\\..*" "\\1" MODULE_NAME ${TEST_FILE})
    add_executable(${MODULE_NAME} ${TEST_FILE})
    list(APPEND UNITTEST_TARGETS ${MODULE_NAME})
endforeach()

set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_STANDARD 17)
set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_EXTENSIONS OFF)

include(GoogleTest)
foreach(T ${UNITTEST_TARGETS})
    target_include_directories(${T} PRIVATE ${PROJECT_SOURCE_DIR}/../)
    target_link_libraries(${T} PRIVATE GTest::GTest GTest::Main ${PROJECT_NAME})
    gtest_discover_tests(${T})
endforeach()

enable_testing()