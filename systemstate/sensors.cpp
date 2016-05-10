#include "sensors.h"
#include <hwhal/context.h>
#include <hwhal/sensors.h>
#include <algorithm>
#include <map>
#include <iostream>

class Monitor {
public:
  Monitor(Sensors *sensors, const Sensors::Sensor& sensor) :
    m_sensors(sensors),
    m_sensor(sensor),
    m_nextId(1) {}

  ~Monitor() { stop(); }

  Sensors::Sensor sensor() const {
    return m_sensor;
  }

  bool hasListener(int id) const {
    return m_listeners.find(id) != m_listeners.end();
  }

  int addListener(const std::function<void(const Sensors::Reading& reading)>& listener) {
    int id = m_nextId++;
    m_listeners.insert(std::make_pair(id, listener));
    return id;
  }

  void removeListener(int id) {
    auto iter = m_listeners.find(id);
    if (iter != m_listeners.end()) {
      m_listeners.erase(iter);
    }
  }

  bool hasListeners() const {
    return !m_listeners.empty();
  }

  bool start() {
    if (!m_sensors->hasSensor(m_sensor)) {
      std::cerr << "Sensor " << m_sensor << " is not available" << std::endl;
      return false;
    }

    bool ret = m_sensors->monitor(m_sensor, [this](const Sensors::Reading& reading) {
	for (auto iter : m_listeners) {
	  iter.second(reading);
	}
      });

    if (!ret) {
      std::cerr << "Failed to add sensor listener for " << m_sensor << std::endl;
    }

    return ret;
  }

  void stop() {
    m_sensors->monitor(m_sensor, nullptr);
  }

private:
  Sensors *m_sensors;
  const Sensors::Sensor m_sensor;
  std::map<int, std::function<void(const Sensors::Reading& reading)>> m_listeners;
  int m_nextId;
};

SensorsHalContainer::SensorsHalContainer(Context *ctx) :
  m_ctx(ctx),
  m_hal(nullptr) {

}

SensorsHalContainer::~SensorsHalContainer() {
  if (m_hal) {
    m_ctx->put(m_hal);
    m_hal = nullptr;
  }

  m_ctx = nullptr;
}

Sensors *SensorsHalContainer::hal() {
  if (!m_hal) {
    m_hal = m_ctx->control<Sensors>(ControlId::Sensors);
  }

  return m_hal;
}

bool SensorsHalContainer::hasSensor(const Sensors::Sensor& sensor) {
  Sensors *s = hal();
  if (!s) {
    return false;
  }

  return s->hasSensor(sensor);
}

int SensorsHalContainer::addMonitor(const Sensors::Sensor& sensor,
				    const std::function<void(const Sensors::Reading& reading)>&
				    listener) {
  Sensors *s = hal();
  if (!s) {
    return -1;
  }

  auto iter = std::find_if(m_monitors.begin(), m_monitors.end(),
			   [sensor](const Monitor *monitor) ->bool {
			     return monitor->sensor() == sensor;
			   });

  if (iter != m_monitors.end()) {
    return (*iter)->addListener(listener);
  }

  Monitor *m = new Monitor(m_hal, sensor);
  int id = m->addListener(listener);
  if (!m->start()) {
    delete m;
    return -1;
  }

  m_monitors.push_back(m);
  return id;
}

void SensorsHalContainer::removeMonitor(const Sensors::Sensor& sensor, int id) {
  Sensors *s = hal();
  if (!s) {
    return;
  }

  auto iter = std::find_if(m_monitors.begin(), m_monitors.end(),
			   [sensor, id](const Monitor *monitor) ->bool {
			     return monitor->sensor() == sensor && monitor->hasListener(id);
			   });

  if (iter != m_monitors.end()) {
    Monitor *m = *iter;
    m->removeListener(id);
    if (!m->hasListeners()) {
      m_monitors.erase(iter);
      m->stop();
      delete m;
    }
  }
}

SensorNode::SensorNode(const std::string& name, systemstate::DirNode *dir,
		       systemstate::Plugin *plugin, SensorsHalContainer *container,
		       const Sensors::Sensor& sensor) :
  FileNode(name, dir, plugin),
  m_container(container),
  m_id(-1),
  m_sensor(sensor) {

}

SensorNode::~SensorNode() {
  m_container = nullptr;
}

bool SensorNode::start() {
  if (m_cb) {
    m_id = m_container->addMonitor(m_sensor, m_cb);
    return m_id != -1;
  }

  return true;
}

void SensorNode::stop() {
  if (m_id != -1) {
    m_container->removeMonitor(m_sensor, m_id);
    m_id = -1;
  }
}

bool AvailableSensors::read(std::stringstream& data) {
  std::map<Sensors::Sensor, std::string> sensors = {
    {Sensors::Proximity, "Proximity"},
    {Sensors::Magnetometer, "Magnetometer"},
    {Sensors::Accelerometer, "Accelerometer"},
    {Sensors::AmbientLight, "AmbientLight"},
  };

  std::list<std::string> available;

  for (auto iter : sensors) {
    if (m_container->hasSensor(iter.first)) {
      available.push_back(iter.second);
    }
  }

  data << std::accumulate(available.begin(), available.end(), std::string(),
			  [](const std::string& a, const std::string& b) ->std::string{
			    return a.empty() ? b : a + " " + b;
			  });
  return true;
}
