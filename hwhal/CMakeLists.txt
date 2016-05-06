add_library(hwhal SHARED
  context.cpp
  plugin.cpp
  )

target_link_libraries(hwhal dl)

set_target_properties(hwhal PROPERTIES SOVERSION 1 VERSION 1)

set(HEADERS
  battery.h
  context.h
  control.h
  display.h
  hwhal.h
  info.h
  keys.h
  lights.h
  plugin.h
  usb.h
  sensors.h
  loopintegration.h
  )

install(TARGETS hwhal LIBRARY DESTINATION /usr/lib)
install(FILES ${HEADERS} DESTINATION /usr/include/hwhal/)
