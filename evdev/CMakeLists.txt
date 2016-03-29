include_directories(../lib)

add_library(hwhal-keys-evdev SHARED
  keys-evdev.cpp
  )

set(HEADERS
  keys-evdev.h
  )

target_link_libraries(hwhal-keys-evdev hwhal pthread)

set_target_properties(hwhal-keys-evdev PROPERTIES SOVERSION 1 VERSION 1)

install(TARGETS hwhal-keys-evdev LIBRARY DESTINATION /usr/lib/)
install(FILES ${HEADERS} DESTINATION /usr/include/hwhal/)
