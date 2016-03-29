#ifndef DISPLAY_H
#define DISPLAY_H

#include "control.h"

class Display : public Control {
public:
  Display() {}
  virtual ~Display() {}

#if 0
  virtual void blank(bool blank) = 0;
  virtual bool isBlank() = 0;
#endif
};

#endif /* DISPLAY_H */
