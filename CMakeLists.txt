cmake_minimum_required(VERSION 2.6)
project(HwHal)
set(CMAKE_CXX_FLAGS "-std=c++0x")

add_subdirectory(lib)
add_subdirectory(plugin)
add_subdirectory(test)
