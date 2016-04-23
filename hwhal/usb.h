#ifndef USB_H
#define USB_H

#include "control.h"
#include <functional>

class Usb : public Control {
public:
  typedef enum {
    None,
    UsbNet,
    Tethering,
  } Mode;

  Usb() {}
  virtual ~Usb() {}

  virtual void addListener(std::function<void(bool)>& listener) = 0;

  virtual bool isCableConnected() = 0;
  virtual bool setMode(const Mode& mode) = 0;
};

#endif /* USB_H */
