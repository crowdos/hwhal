#ifndef SENSORS_H
#define SENSORS_H

#include "control.h"
#include <functional>

class Sensors : public Control {
public:
  typedef enum {
    Proximity,
    Magnetometer,
    Accelerometer,
    AmbientLight,
  } Sensor;

  typedef enum {
    Invalid = -1,
    Unknown = 0,
    Valid = 1,
  } Validity;

  class Reading {
  public:
    float data[3];
    Validity valid;
  };

  Sensors() {}
  virtual ~Sensors() {}

  virtual bool hasSensor(const Sensor& sensor) = 0;
  virtual bool monitor(const Sensor& sensor,
		       const std::function<void(const Reading& reading)>& listener) = 0;
};

#endif /* SENSORS_H */
