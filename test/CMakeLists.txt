include_directories(../lib)

add_executable(test test.cpp)
target_link_libraries(test hwhal)
