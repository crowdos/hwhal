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

class SensorReading : public SensorNode {
public:
  SensorReading(const std::string& name, systemstate::DirNode *dir, systemstate::Plugin *plugin,
		SensorsHalContainer *container, const Sensors::Sensor sensor,
		const std::function<void(const Sensors::Reading&, std::stringstream&)>& formatter) :
    SensorNode(name, dir, plugin, container, sensor),
    m_formatter(formatter) {}

  bool read(std::stringstream& data) override;

  static void extract(int n, const Sensors::Reading& r, std::stringstream& s);

private:
  std::function<void(const Sensors::Reading&, std::stringstream&)> m_formatter;
};

#endif /* SENSORS_PLUGIN_H */
