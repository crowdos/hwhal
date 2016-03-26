#ifndef LIGHTS_H
#define LIGHTS_H

#include "control.h"

class Lights : public Control {
public:
  Lights() {}
  virtual ~Lights() {}

  virtual int minBacklightBrightness() = 0;
  virtual int maxBacklightBrightness() = 0;

  virtual int backlightBrightness() = 0;
  virtual void setBacklightBrightness(int brightness) = 0;
};

#endif /* LIGHTS_H */
