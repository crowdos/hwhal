#ifndef CONTROL_H
#define CONTROL_H

#include <string>

enum class ControlId {
  Info,
  Display,
  Usb,
  Battery,
  Lights,
  Keys,
  Sensors,
};

class Control {
public:
  virtual ~Control() {}
  virtual bool init() { return true; }
  virtual void destroy() { delete this; }

protected:
  Control() {}
};

#endif /* CONTROL_H */
