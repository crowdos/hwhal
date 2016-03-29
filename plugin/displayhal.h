#ifndef DISPLAY_HAL_H
#define DISPLAY_HAL_H

#include "hwhal/display.h"

class DisplayHal : public Display {
public:
  DisplayHal();

  void blank(bool blank);
  bool isBlank();

private:
  bool m_blank;
};

#endif /* DISPLAY_HAL_H */
