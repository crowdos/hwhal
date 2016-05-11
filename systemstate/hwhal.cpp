#include "hwhal.h"
#include <loop-integrations/asio.h>
#include <hwhal/context.h>
#include <hwhal/control.h>
#include <hwhal/display.h>
#include <hwhal/lights.h>
#include <hwhal/info.h>
#include <hwhal/usb.h>
#include "sensors.h"

namespace ph = std::placeholders;

bool ControlNodeInterface::read(std::string& data) {
  std::stringstream s;

  if (read(s)) {
    data = s.str();
    return true;
  }

  return false;
}

template <class T> ControlNode<T>::ControlNode(const std::string& name, systemstate::DirNode *dir,
					       systemstate::Plugin *plugin,
					       Context *ctx, const ControlId& id) :
  FileNode(name, dir, plugin),
  m_id(id),
  m_ctx(ctx),
  m_ctl(nullptr) {
}

template <class T> bool ControlNode<T>::start() {
  if (!m_ctx) {
    return false;
  }

  if (m_ctl) {
    return false;
  }

  m_ctl = m_ctx->control<T>(m_id);

  return m_ctl != nullptr;
}

template <class T> void ControlNode<T>::stop() {
  if (m_ctx && m_ctl) {
    Control *c = dynamic_cast<Control *>(m_ctl);
    if (c) {
      m_ctx->put(c);
    }
  }
}

bool ScreenBrightness::read(std::stringstream& data) {
  data << control()->backlightBrightness();
  return true;
}

bool ScreenBrightness::write(const std::string& data) {
  if (data.empty()) {
    return false;
  }

  int level;

  try {
    level = std::stoi(data);
  } catch (...) {
    // Oops!
    return false;
  }

  if (level < control()->minBacklightBrightness() || level > control()->maxBacklightBrightness()) {
    return false;
  }

  control()->setBacklightBrightness(level);

  return true;
}

bool ScreenBrightnessMin::read(std::stringstream& data) {
  data << control()->minBacklightBrightness();
  return true;
}

bool ScreenBrightnessMax::read(std::stringstream& data) {
  data << control()->maxBacklightBrightness();
  return true;
}

bool DeviceMaker::read(std::stringstream& data) {
  data << control()->maker();
  return true;
}

bool DeviceModel::read(std::stringstream& data) {
  data << control()->model();
  return true;
}

bool DeviceCodeName::read(std::stringstream& data) {
  data << control()->codeName();
  return true;
}

bool UsbConnected::read(std::stringstream& data) {
  data << (m_connected ? "1" : "0");
  return true;
}

bool UsbConnected::start() {
  if (ControlNode::start()) {

    control()->addListener([this](bool connected) {
	if (connected != m_connected) {
	  m_connected = connected;
	  std::stringstream data;
	  read(data);
	  dataChanged(data.str());
	}
      });
    m_connected = control()->isCableConnected();
    return true;
  }

  return false;
}

void UsbConnected::stop() {
  if (control()) {
    control()->addListener(nullptr);
  }

  ControlNode::stop();
}

// Now the plugin
class HwHalPlugin : public systemstate::Plugin {
public:
  HwHalPlugin(boost::asio::io_service& service);
  ~HwHalPlugin();

  void init(systemstate::DirNode *root);
  bool start(systemstate::FileNode *node);
  void stop(systemstate::FileNode *node);
  bool read(systemstate::FileNode *node, std::string& data);
  bool write(systemstate::FileNode *node, const std::string& data);

private:
  LoopIntegration *m_loop;
  Context *m_ctx;
  SensorsHalContainer *m_sensors;
};

HwHalPlugin::HwHalPlugin(boost::asio::io_service& service) :
  m_loop(new LoopIntegrationAsio(service)),
  m_ctx(nullptr),
  m_sensors(nullptr) {

}

HwHalPlugin::~HwHalPlugin() {
  delete m_loop;
  m_loop = nullptr;

  delete m_sensors;
  m_sensors = nullptr;

  delete m_ctx;
  m_ctx = nullptr;
}

void HwHalPlugin::init(systemstate::DirNode *root) {
  m_ctx = Context::create(m_loop);
  if (!m_ctx) {
    return;
  }

  systemstate::DirNode *screen = root->appendDir("Screen");

  systemstate::DirNode *brightness = screen->appendDir("Brightness");
  brightness->appendFile(new ScreenBrightness(brightness, this, m_ctx));
  brightness->appendFile(new ScreenBrightnessMin(brightness, this, m_ctx));
  brightness->appendFile(new ScreenBrightnessMax(brightness, this, m_ctx));

  systemstate::DirNode *device = root->appendDir("Device");
  device->appendFile(new DeviceMaker(device, this, m_ctx));
  device->appendFile(new DeviceModel(device, this, m_ctx));
  device->appendFile(new DeviceCodeName(device, this, m_ctx));

  systemstate::DirNode *usb = root->appendDir("USB");
  usb->appendFile(new UsbConnected(usb, this, m_ctx));

  if (!m_sensors) {
    m_sensors = new SensorsHalContainer(m_ctx);
  }

  systemstate::DirNode *sensors = root->appendDir("Sensors");
  sensors->appendFile(new AvailableSensors(sensors, this, m_sensors));

  systemstate::DirNode *accelerometer = sensors->appendDir("Accelerometer");
  accelerometer->appendFile(new SensorReading("Reading", accelerometer, this, m_sensors, Sensors::Accelerometer,
					      std::bind(&SensorReading::extract, -1, ph::_1, ph::_2)));
  accelerometer->appendFile(new SensorReading("X", accelerometer, this, m_sensors, Sensors::Accelerometer,
					      std::bind(&SensorReading::extract, 0, ph::_1, ph::_2)));
  accelerometer->appendFile(new SensorReading("Y", accelerometer, this, m_sensors, Sensors::Accelerometer,
					      std::bind(&SensorReading::extract, 1, ph::_1, ph::_2)));
  accelerometer->appendFile(new SensorReading("Z", accelerometer, this, m_sensors, Sensors::Accelerometer,
					      std::bind(&SensorReading::extract, 2, ph::_1, ph::_2)));

  systemstate::DirNode *magnetometer = sensors->appendDir("Magnetometer");
  magnetometer->appendFile(new SensorReading("Reading", magnetometer, this, m_sensors, Sensors::Magnetometer,
					     std::bind(&SensorReading::extract, -1, ph::_1, ph::_2)));
  magnetometer->appendFile(new SensorReading("X", magnetometer, this, m_sensors, Sensors::Magnetometer,
					     std::bind(&SensorReading::extract, 0, ph::_1, ph::_2)));
  magnetometer->appendFile(new SensorReading("Y", magnetometer, this, m_sensors, Sensors::Magnetometer,
					     std::bind(&SensorReading::extract, 1, ph::_1, ph::_2)));
  magnetometer->appendFile(new SensorReading("Z", magnetometer, this, m_sensors, Sensors::Magnetometer,
					     std::bind(&SensorReading::extract, 2, ph::_1, ph::_2)));
}

bool HwHalPlugin::start(systemstate::FileNode *node) {
  if (!m_ctx) {
    return false;
  }

  return dynamic_cast<ControlNodeInterface *>(node)->start();
}

void HwHalPlugin::stop(systemstate::FileNode *node) {
  dynamic_cast<ControlNodeInterface *>(node)->stop();
}

bool HwHalPlugin::read(systemstate::FileNode *node, std::string& data) {
  return dynamic_cast<ControlNodeInterface *>(node)->read(data);
}

bool HwHalPlugin::write(systemstate::FileNode *node, const std::string& data) {
  return dynamic_cast<ControlNodeInterface *>(node)->write(data);
}

REGISTER_STATE_PLUGIN(HwHalPlugin);
