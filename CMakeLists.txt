cmake_minimum_required(VERSION 2.6)
project(HwHal)
set(CMAKE_CXX_FLAGS "-std=c++0x")

add_subdirectory(hwhal)
add_subdirectory(loop-integrations)
add_subdirectory(evdev)
add_subdirectory(plugin)
add_subdirectory(test)
