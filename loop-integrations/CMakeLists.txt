include_directories(../)

find_package(Boost REQUIRED system)
include_directories(${Boost_INCLUDE_DIRS})

add_library(hwhal-loopintegration-asio SHARED
  asio.cpp
  )

set(HEADERS
  asio.h
  )

target_link_libraries(hwhal-loopintegration-asio pthread ${Boost_LIBRARIES})

set_target_properties(hwhal-loopintegration-asio PROPERTIES SOVERSION 1 VERSION 1)

install(TARGETS hwhal-loopintegration-asio LIBRARY DESTINATION /usr/lib/)
install(FILES ${HEADERS} DESTINATION /usr/include/hwhal/loopintegration/)
