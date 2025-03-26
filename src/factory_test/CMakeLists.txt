cmake_minimum_required(VERSION 3.0.2)

project(factory_test)
add_definitions(-std=c++2a -O3)
find_package(GTest REQUIRED)

include_directories(
    ${PROJECT_SOURCE_DIR}/../../
)

add_executable(${PROJECT_NAME} main.cc derivedA.cc derivedB.cc)
target_link_libraries(
    ${PROJECT_NAME}
    core
)