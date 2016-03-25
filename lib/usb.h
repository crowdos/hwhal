#ifndef USB_H
#define USB_H

#include "control.h"

class Context;

class Usb : public Control {
public:
  Usb(Context *ctx);
  ~Usb();
};

#endif /* USB_H */
