#include "context.h"
#include "display.h"
#include "usb.h"
#include "info.h"
#include "battery.h"

Context *Context::create() {
  Context *ctx = new Context;

  if (!ctx->init()) {
    delete ctx;
    ctx = nullptr;
  }

  return ctx;
}

Context::Context() {

}

Context::~Context() {
  for (auto it : m_controls) {
    delete it.second;
  }

  m_controls.clear();
}

bool Context::init() {
  // TODO:

  return true;
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
    m_controls.insert(std::make_pair(name, t));
    return t;
  } catch (...) {
    return nullptr;
  }
}
