#ifndef DISPLAY_HAL_H
#define DISPLAY_HAL_H

#include "display.h"

class DisplayHal : public Display {
  int brightness();
  void setBrightness(int brightness);
  void blank(bool blank);
  bool isBlank();
};

#endif /* DISPLAY_HAL_H */
