#ifndef INFO_HAL_H
#define INFO_HAL_H

#include "hwhal/info.h"

class InfoHal : public Info {
  std::string maker();
  std::string model();
  std::string codeName();
};

#endif /* INFO_HAL_H */
