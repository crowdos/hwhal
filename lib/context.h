#ifndef CONTEXT_H
#define CONTEXT_H

#include <string>
#include <list>

class Wrapper;
class Control;
class Display;
class Usb;
class Info;
class Battery;
class Lights;
class Plugin;

class Context {
public:
  static Context *create(bool test = false);

  ~Context();

  Display *display();
  Usb *usb();
  Info *info();
  Battery *battery();
  Lights *lights();

  void put(Control *control);

private:
  Context();
  bool init(bool test);

  template <typename T> T *control(const std::string& name);
  std::list<Wrapper *> m_controls;
  Plugin *m_plugin;
};

#endif /* CONTEXT_H */
