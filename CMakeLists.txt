cmake_minimum_required(VERSION 2.8)

add_subdirectory(libcoro)
include_directories("${CMAKE_CURRENT_BINARY_DIR}/libcoro" ${CMAKE_CURRENT_BINARY_DIR})

project (cpp_generator)

add_executable(demo demo.cpp cpp_generator.cpp cpp_generator.h)
target_link_libraries(demo coro)
