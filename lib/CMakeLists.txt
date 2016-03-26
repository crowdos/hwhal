add_library(hwhal SHARED
  context.cpp
  plugin.cpp
  )

target_link_libraries(hwhal dl)

set_target_properties(hwhal PROPERTIES SOVERSION 1 VERSION 1)

install(TARGETS hwhal LIBRARY DESTINATION /usr/lib)
