#ifndef INFO_H
#define INFO_H

#include "control.h"
#include <string>

class Context;

class Info : public Control {
public:
  Info(Context *ctx);
  ~Info();

  std::string maker();
  std::string model();
  std::string codeName();
};

#endif /* INFO_H */
