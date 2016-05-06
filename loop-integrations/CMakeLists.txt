include_directories(../)

find_package(Boost REQUIRED system)

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB glib-2.0 REQUIRED)

include_directories(${Boost_INCLUDE_DIRS})
include_directories(${GLIB_INCLUDE_DIRS})

add_library(hwhal-loopintegration-asio SHARED
  asio.cpp
  )

add_library(hwhal-loopintegration-glib SHARED
  glib.cpp
  )

set(HEADERS
  asio.h
  glib.h
  )

target_link_libraries(hwhal-loopintegration-asio pthread ${Boost_LIBRARIES})
set_target_properties(hwhal-loopintegration-asio PROPERTIES SOVERSION 1 VERSION 1)

target_link_libraries(hwhal-loopintegration-glib ${GLIB_LIBRARIES})
set_target_properties(hwhal-loopintegration-glib PROPERTIES SOVERSION 1 VERSION 1)

install(TARGETS hwhal-loopintegration-asio LIBRARY DESTINATION /usr/lib/)
install(TARGETS hwhal-loopintegration-glib LIBRARY DESTINATION /usr/lib/)
install(FILES ${HEADERS} DESTINATION /usr/include/hwhal/loopintegration/)
