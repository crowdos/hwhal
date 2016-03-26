#include "context.h"
#include "plugin.h"
#include "hwhal.h"

#include "display.h"
#include "usb.h"
#include "info.h"
#include "battery.h"
#include "lights.h"

#include <algorithm>
#include <cassert>

class Wrapper {
public:
  Wrapper(Control *ctl, const std::string& name) :
    m_ctl(ctl),
    m_name(name),
    m_ref(1) {}

  void ref() { ++m_ref; }
  void unref() { --m_ref; }
  int refCount() { return m_ref; }
  std::string name() { return m_name; }
  Control *control() { return m_ctl; }

private:
  int m_ref;
  std::string m_name;
  Control *m_ctl;
};

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
  std::for_each(m_controls.begin(), m_controls.end(),
		[this] (Wrapper *w) {
		  m_plugin->hal()->put(w->control());
		  delete w;
		});

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

Lights *Context::lights() {
  return control<Lights>("lights");
}

template <typename T> T *Context::control(const std::string& name) {
  auto it = std::find_if(m_controls.begin(), m_controls.end(),
			 [name] (Wrapper *w) {
			   return w->name() == name;
			 });

  if (it == m_controls.end()) {
    Control *ctl = m_plugin->hal()->get(name);
    if (!ctl) {
      return nullptr;
    }

    Wrapper *w = new Wrapper(ctl, name);
    m_controls.push_back(w);
    it = m_controls.end();
    --it;
  } else {
    (*it)->ref();
  }

  return dynamic_cast<T *>((*it)->control());
}

void Context::put(Control *control) {
  auto iter = std::find_if(m_controls.begin(), m_controls.end(),
			   [control](Wrapper *w) { return w->control() == control; });

  assert(iter != m_controls.end());

  Wrapper *w = *iter;

  w->unref();
  if (w->refCount() == 0) {
    m_controls.remove(w);
    m_plugin->hal()->put(w->control());
    delete w;
  }
}
