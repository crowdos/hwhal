#ifndef DISPLAY_H
#define DISPLAY_H

#include "control.h"

class Display : public Control {
public:
  Display() {}
  virtual ~Display() {}

  virtual int minBrightness() = 0;
  virtual int maxBrightness() = 0;

  virtual int brightness() = 0;
  virtual void setBrightness(int brightness) = 0;

  virtual void blank(bool blank) = 0;
  virtual bool isBlank() = 0;
};

#endif /* DISPLAY_H */
