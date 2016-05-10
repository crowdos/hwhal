#ifndef SENSORS_PLUGIN_H
#define SENSORS_PLUGIN_H

#include <hwhal/sensors.h>
#include <systemstate/stateplugin.h>
#include <list>
#include "hwhal.h"

class Context;
class Monitor;

class SensorsHalContainer {
public:
  SensorsHalContainer(Context *ctx);
  ~SensorsHalContainer();

  bool hasSensor(const Sensors::Sensor& sensor);
  int addMonitor(const Sensors::Sensor& sensor,
		 const std::function<void(const Sensors::Reading& reading)>& listener);
  void removeMonitor(const Sensors::Sensor& sensor, int id);

private:
  Sensors *hal();

  Context *m_ctx;
  Sensors *m_hal;

  std::list<Monitor *> m_monitors;
};

class SensorNode : public systemstate::FileNode, public ControlNodeInterface {
public:
  SensorNode(const std::string& name, systemstate::DirNode *dir, systemstate::Plugin *plugin,
	     SensorsHalContainer *container, const Sensors::Sensor& sensor);
  virtual ~SensorNode();

  bool start() override;
  void stop() override;

  SensorsHalContainer *container() const { return m_container; }

  const Sensors::Reading& reading() const { return m_reading; }

private:
  void setReading(const Sensors::Reading& reading);

  Sensors::Reading m_reading;
  SensorsHalContainer *m_container;
  int m_id;
  Sensors::Sensor m_sensor;
  std::function<void(const Sensors::Reading& reading)> m_cb;
};

class AvailableSensors : public ControlNodeInterface, public systemstate::FileNode {
public:
  AvailableSensors(systemstate::DirNode *dir, systemstate::Plugin *plugin,
		   SensorsHalContainer *container) :
    FileNode("Available", dir, plugin), m_container(container) {}
  ~AvailableSensors() { m_container = nullptr; }

  bool start() override { return true; }
  void stop() override {}
  bool read(std::stringstream& data) override;

private:
  SensorsHalContainer *m_container;
};

class AccelerometerReading : public SensorNode {
public:
  AccelerometerReading(systemstate::DirNode *dir, systemstate::Plugin *plugin,
		   SensorsHalContainer *container) :
    SensorNode("Reading", dir, plugin, container, Sensors::Accelerometer) {}

  bool read(std::stringstream& data) override;
};

class AccelerometerX : public SensorNode {
public:
  AccelerometerX(systemstate::DirNode *dir, systemstate::Plugin *plugin,
		   SensorsHalContainer *container) :
    SensorNode("X", dir, plugin, container, Sensors::Accelerometer) {}

  bool read(std::stringstream& data) override;
};

class AccelerometerY : public SensorNode {
public:
AccelerometerY(systemstate::DirNode *dir, systemstate::Plugin *plugin,
		   SensorsHalContainer *container) :
    SensorNode("Y", dir, plugin, container, Sensors::Accelerometer) {}

  bool read(std::stringstream& data) override;
};

class AccelerometerZ : public SensorNode {
public:
AccelerometerZ(systemstate::DirNode *dir, systemstate::Plugin *plugin,
		   SensorsHalContainer *container) :
    SensorNode("Z", dir, plugin, container, Sensors::Accelerometer) {}

  bool read(std::stringstream& data) override;
};

#endif /* SENSORS_PLUGIN_H */
