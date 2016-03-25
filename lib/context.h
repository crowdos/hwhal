#ifndef CONTEXT_H
#define CONTEXT_H

#include <string>
#include <map>

class Control;
class Display;
class Usb;
class Info;
class Battery;
class Plugin;

class Context {
public:
  static Context *create(bool test = false);

  ~Context();

  Display *display();
  Usb *usb();
  Info *info();
  Battery *battery();

private:
  Context();
  bool init(bool test);

  template <typename T> T *control(const std::string& name);
  std::map<std::string, Control *> m_controls;
  Plugin *m_plugin;
};

#endif /* CONTEXT_H */
