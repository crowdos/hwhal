#ifndef CONTEXT_H
#define CONTEXT_H

#include <string>
#include <map>

class Control;
class Display;
class Usb;
class Info;
class Battery;

class Context {
public:
  static Context *create();

  ~Context();

  Display *display();
  Usb *usb();
  Info *info();
  Battery *battery();

private:
  Context();
  bool init();

  template <typename T> T *control(const std::string& name);
  std::map<std::string, Control *> m_controls;
};

#endif /* CONTEXT_H */
