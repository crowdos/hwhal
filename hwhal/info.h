#ifndef INFO_H
#define INFO_H

#include "control.h"
#include <string>

class Info : public Control {
public:
  Info() {}
  virtual ~Info() {}

  virtual std::string maker() = 0;
  virtual std::string model() = 0;
  virtual std::string codeName() = 0;
};

#endif /* INFO_H */
