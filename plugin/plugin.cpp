#include "hwhal.h"
#include "control.h"
#include "infohal.h"
#include "usbhal.h"
#include "batteryhal.h"
#include "displayhal.h"
#include "lightshal.h"
#include <iostream>

class HAL : public HwHal {
  int version() {
    return HWHAL_VERSION_CURRENT;
  }

  bool init() {
    return true;
  }

  void destroy() {
    delete this;
  }

  Control *get(const std::string& name) {
    Control *ctl = nullptr;

    if (name == "info") {
      ctl = new InfoHal;
    } else if (name == "usb") {
      ctl = new UsbHal;
    } else if (name == "battery") {
      ctl = new BatteryHal;
    } else if (name == "display") {
      ctl = new DisplayHal;
    } else if (name == "lights") {
      ctl = new LightsHal;
    } else {
      std::cerr << "Unknown hal ID " << name << std::endl;
    }

    return ctl;
  }

  void put(Control *control) {
    control->destroy();
  }
};

extern "C" {
  HwHal *__init() {
    return new HAL;
  }
}
