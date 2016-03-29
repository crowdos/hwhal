#ifndef LIGHTS_HAL_H
#define LIGHTS_HAL_H

#include "hwhal/lights.h"

class LightsHal : public Lights {
public:
  LightsHal();

  int minBacklightBrightness();
  int maxBacklightBrightness();
  int backlightBrightness();
  void setBacklightBrightness(int brightness);

private:
  int m_brightness;
};

#endif /* LIGHTS_HAL_H */
