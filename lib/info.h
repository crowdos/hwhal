#ifndef INFO_H
#define INFO_H

#include "control.h"

class Context;

class Info : public Control {
public:
  Info(Context *ctx);
  ~Info();
};

#endif /* INFO_H */
