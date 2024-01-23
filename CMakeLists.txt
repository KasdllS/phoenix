cmake_minimum_required(VERSION 3.10)

project(pheonix)

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
    target_include_directories(${T} PRIVATE ${PROJECT_SOURCE_DIR})
    target_link_libraries(${T} PRIVATE GTest::GTest GTest::Main)
    gtest_discover_tests(${T})
endforeach()

enable_testing()