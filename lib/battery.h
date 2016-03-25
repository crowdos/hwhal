#ifndef BATTERY_H
#define BATTERY_H

#include "control.h"

class Context;

class Battery : public Control {
public:
  Battery(Context *ctx);
  ~Battery();
};

#endif /* BATTERY_H */
