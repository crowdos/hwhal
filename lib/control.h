#ifndef CONTROL_H
#define CONTROL_H

#include <string>

class Context;

class Control {
public:
  virtual ~Control();

  Context *context();

  bool init(const std::string& name);

protected:
  Control(Context *ctx);

private:
  Context *m_context;
};

#endif /* CONTROL_H */
