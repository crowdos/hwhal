#ifndef SENSORS_H
#define SENSORS_H

#include "control.h"
#include <functional>
#include <vector>

class Sensors : public Control {
public:
  typedef enum {
    Proximity,
    Magnetometer,
    Accelerometer,
    AmbientLight,
  } Sensor;

  Sensors() {}
  virtual ~Sensors() {}

  virtual bool hasSensor(const Sensor& sensor) = 0;
  virtual bool monitor(const Sensor& sensor,
		       const std::function<void(const std::vector<int>&)>& listener) = 0;
};

#endif /* SENSORS_H */
