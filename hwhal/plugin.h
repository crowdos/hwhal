#ifndef PLUGIN_H
#define PLUGIN_H

#include <string>

#define HWHAL_SYM                      "__init"

class HwHal;

class Plugin {
public:
  Plugin();
  virtual ~Plugin();

  bool load();

  HwHal *hal();

private:
  void unload();

  virtual const char *path();

  void *m_handle;
  HwHal *m_hal;
};

class TestPlugin : public Plugin {
  const char *path();
};

#endif /* PLUGIN_H */
