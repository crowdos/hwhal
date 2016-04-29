#ifndef PLUGIN_H
#define PLUGIN_H

#include <string>

#define HWHAL_SYM                      "__init"

class HwHal;
class LoopIntegration;

class Plugin {
public:
  Plugin(LoopIntegration *loop);
  virtual ~Plugin();

  bool load();

  HwHal *hal();

  LoopIntegration *loopIntegration() const { return m_loop; }

private:
  void unload();

  virtual const char *path();

  void *m_handle;
  HwHal *m_hal;
  LoopIntegration *m_loop;
};

class TestPlugin : public Plugin {
public:
  TestPlugin(LoopIntegration *loop) : Plugin(loop) {}

private:
  const char *path();
};

#endif /* PLUGIN_H */
