include_directories(../)

add_library(hwhal-test MODULE
  infohal.cpp
  displayhal.cpp
  usbhal.cpp
  batteryhal.cpp
  lightshal.cpp
  plugin.cpp
  )
