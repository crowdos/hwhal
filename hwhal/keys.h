#ifndef KEYS_H
#define KEYS_H

#include "control.h"
#include <functional>

#define KEY_PRESSED            '1'
#define KEY_RELEASED           '0'

class Keys : public Control {
public:
  enum class Key {
    Power,
    VolumeUp,
    VolumeDown,
  };

  Keys() {}
  virtual ~Keys() {}

  virtual bool monitor(const Key& key, const std::function<void(bool)>& listener) = 0;
};

#endif /* KEYS_H */
