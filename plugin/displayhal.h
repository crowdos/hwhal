#ifndef DISPLAY_HAL_H
#define DISPLAY_HAL_H

#include "display.h"

class DisplayHal : public Display {
public:
  DisplayHal();

  int minBrightness();
  int maxBrightness();
  int brightness();
  void setBrightness(int brightness);
  void blank(bool blank);
  bool isBlank();

private:
  int m_brightness;
  bool m_blank;
};

#endif /* DISPLAY_HAL_H */
