#ifndef HWHAL_KEYS_EVDEV_H
#define HWHAL_KEYS_EVDEV_H

#include "hwhal/keys.h"
#include <map>

class HwHalKeysEvdevState;

class HwHalKeysEvdev : public Keys {
public:
  HwHalKeysEvdev();
  ~HwHalKeysEvdev();

  int fd(const Key& key);

  bool monitor(const Key& key, bool enable);

private:
  HwHalKeysEvdevState *findKey(const Key& key);

  std::map<Keys::Key, HwHalKeysEvdevState *> m_keys;
};

#endif /* HWHAL_KEYS_EVDEV_H */
