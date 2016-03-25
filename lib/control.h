#ifndef CONTROL_H
#define CONTROL_H

#include <string>

class Control {
public:
  virtual ~Control() {}
  virtual bool init() { return true; }
  virtual void destroy() { delete this; }

protected:
  Control() {}
};

#endif /* CONTROL_H */
