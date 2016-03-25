#include "plugin.h"
#include <dlfcn.h>
#include <cassert>
#include <sstream>
#include <iostream>
#include "hwhal.h"

#define HWHAL_NAME                     "/usr/lib/hwhal/libhwhal.so"
#define HWHAL_NAME_TEST                "libhwhal.so"
typedef HwHal *(*__init)();

Plugin::Plugin() :
  m_handle(nullptr),
  m_hal(nullptr) {

}

Plugin::~Plugin() {
  unload();
}

bool Plugin::load() {
  assert(m_handle == nullptr);
  assert(m_hal == nullptr);

  m_handle = dlopen(path(), RTLD_LAZY);
  if (!m_handle) {
    std::cerr << "Failed to load hwhal: " << dlerror() << std::endl;
    return false;
  }

  __init init = (__init)dlsym(m_handle, HWHAL_SYM);
  if (!init) {
    std::cerr << "Failed to load hwhal: " << dlerror() << std::endl;
    return false;
  }

  m_hal = init();

  assert(m_hal != nullptr);

  return m_hal->init();
}

void Plugin::unload() {
  if (m_hal) {
    m_hal->destroy();
    m_hal = nullptr;
  }

  if (m_handle) {
    dlclose(m_handle);
    m_handle = nullptr;
  }
}

const char *Plugin::path() {
  return HWHAL_NAME;
}

const char *TestPlugin::path() {
  return HWHAL_NAME_TEST;
}
