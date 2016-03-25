#ifndef HW_HAL_H
#define HW_HAL_H

class HwHal {
public:
  virtual ~HwHal();
  virtual bool init() = 0;
  virtual void destroy() = 0;

protected:
  HwHal();
};

#endif /* HW_HAL_H */
