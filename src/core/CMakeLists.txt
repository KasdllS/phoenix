cmake_minimum_required(VERSION 3.10)

project(core)

find_package(GTest REQUIRED)

file (GLOB_RECURSE SOURCE_FILES ${PROJECT_SOURCE_DIR}/*.cpp)

add_library(
    ${PROJECT_NAME}
    SHARED
    ${SOURCE_FILES}
    ${PLATFORM_FILES}
)
target_include_directories(
    ${PROJECT_NAME}
    PUBLIC
    ${PROJECT_SOURCE_DIR}/../
)

if (UNIX)
target_link_libraries(
    ${PROJECT_NAME}
    PUBLIC
    pthread
    dl
)
elseif (WIN32)
endif()

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

# gtest单元测试
file(GLOB_RECURSE TEST_FILES  ${PROJECT_SOURCE_DIR}/*_test.cc)
foreach(TEST_FILE ${TEST_FILES})
    string(REGEX REPLACE ".+/(.+)\\..*" "\\1" MODULE_NAME ${TEST_FILE})
    add_executable(${PROJECT_NAME}.${MODULE_NAME} ${TEST_FILE})
    list(APPEND TEST_TARGETS ${PROJECT_NAME}.${MODULE_NAME})
endforeach()
include(GoogleTest)
foreach(T ${TEST_TARGETS})
    target_link_libraries(${T} PRIVATE ${PROJECT_NAME})
endforeach()

enable_testing()