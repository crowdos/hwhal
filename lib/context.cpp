#include "context.h"
#include "plugin.h"
#include "display.h"
#include "usb.h"
#include "info.h"
#include "battery.h"

Context *Context::create(bool test) {
  Context *ctx = new Context;

  if (!ctx->init(test)) {
    delete ctx;
    ctx = nullptr;
  }

  return ctx;
}

Context::Context() :
  m_plugin(nullptr) {

}

Context::~Context() {
  for (auto it : m_controls) {
    delete it.second;
  }

  m_controls.clear();

  if (m_plugin) {
    delete m_plugin;
    m_plugin = nullptr;
  }
}

bool Context::init(bool test) {
  m_plugin = test ? new TestPlugin : new Plugin;
  return m_plugin->load();
}

Display *Context::display() {
  return control<Display>("display");
}

Usb *Context::usb() {
  return control<Usb>("usb");
}

Info *Context::info() {
  return control<Info>("info");
}

Battery *Context::battery() {
  return control<Battery>("battery");
}

template <typename T> T *Context::control(const std::string& name) {
  try {
    return dynamic_cast<T *>(m_controls.at(name));
  } catch (const std::out_of_range& ex) {
    T *t = new T(this);
    if (!t->init(name)) {
      delete t;
      return nullptr;
    }

    m_controls.insert(std::make_pair(name, t));
    return t;
  } catch (...) {
    return nullptr;
  }
}
