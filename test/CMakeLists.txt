include_directories(../)

add_executable(test-app test.cpp)
target_link_libraries(test-app hwhal)
