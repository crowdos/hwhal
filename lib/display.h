#ifndef DISPLAY_H
#define DISPLAY_H

#include "control.h"

class Context;

class Display : public Control {
public:
  Display(Context *ctx);
  ~Display();
};

#endif /* DISPLAY_H */
