#ifndef DISPLAY_H
#define DISPLAY_H

#include "control.h"

class Context;

class Display : public Control {
public:
  Display(Context *ctx);
  ~Display();

  int brightness();
  void setBrightness(int brightness);

  void blank(bool blank);
  bool isBlank();
};

#endif /* DISPLAY_H */
