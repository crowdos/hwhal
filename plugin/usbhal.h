#ifndef USB_HAL_H
#define USB_HAL_H

#include "hwhal/usb.h"

class UsbHal : public Usb {
  void addListener(std::function<void(bool)>& listener);
  bool isCableConnected();
  bool setMode(const Mode& mode);
};

#endif /* USB_HAL_H */
