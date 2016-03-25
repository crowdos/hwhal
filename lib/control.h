#ifndef CONTROL_H
#define CONTROL_H

class Context;

class Control {
public:
  virtual ~Control();

  Context *context();

protected:
  Control(Context *ctx);

private:
  Context *m_context;
};

#endif /* CONTROL_H */
