#ifndef CONTEXT_H
#define CONTEXT_H

#include <string>
#include <list>
#include "control.h"

class Wrapper;
class Control;
class Plugin;
class LoopIntegration;

class Context {
public:
  static Context *create(LoopIntegration *loop, bool test = false);

  ~Context();

  template <typename T> T *control(const ControlId& id) {
    Control *ctl = findControl(id);
    return ctl ? dynamic_cast<T *>(ctl) : nullptr;
  }

  void put(Control *control);

private:
  Context();
  Control *findControl(const ControlId& id);

  bool init(LoopIntegration *loop, bool test);

  std::list<Wrapper *> m_controls;
  Plugin *m_plugin;
};

#endif /* CONTEXT_H */
