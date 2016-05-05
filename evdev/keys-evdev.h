#ifndef HWHAL_KEYS_EVDEV_H
#define HWHAL_KEYS_EVDEV_H

#include "hwhal/keys.h"
#include <functional>
#include <map>

class HwHalKeysEvdevState;
class LoopIntegration;

class HwHalKeysEvdev : public Keys {
public:
  HwHalKeysEvdev(LoopIntegration *loop);
  ~HwHalKeysEvdev();

  bool monitor(const Key& key, const std::function<void(bool)>& listener);

private:
  HwHalKeysEvdevState *findKey(const Key& key);

  LoopIntegration *m_loop;
  std::map<Keys::Key, HwHalKeysEvdevState *> m_keys;
};

#endif /* HWHAL_KEYS_EVDEV_H */
