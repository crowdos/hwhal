include_directories(../)
add_library(systemstate-hwhal MODULE
  hwhal.cpp
  sensors.cpp
  )

target_link_libraries(systemstate-hwhal hwhal hwhal-loopintegration-asio -lsystemstate)
set_target_properties(systemstate-hwhal PROPERTIES OUTPUT_NAME hwhal)
install(TARGETS systemstate-hwhal LIBRARY DESTINATION /usr/lib/systemstate/plugins/)
install(FILES hwhal.ini DESTINATION /usr/lib/systemstate/plugins/)
