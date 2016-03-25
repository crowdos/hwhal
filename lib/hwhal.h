#ifndef HW_HAL_H
#define HW_HAL_H

#include <string>

#define HWHAL_VERSION_CURRENT          0x1

class Control;

class HwHal {
public:
  virtual bool init() = 0;
  virtual void destroy() = 0;
  virtual int version() = 0;

  virtual Control *get(const std::string& name) = 0;
  virtual void put(Control *control) = 0;

protected:
  HwHal() {}
  virtual ~HwHal() {}
};

#endif /* HW_HAL_H */
