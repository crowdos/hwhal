#ifndef KEYS_H
#define KEYS_H

#include "control.h"

#define KEY_PRESSED            "1"
#define KEY_RELEASED           "0"

class Keys : public Control {
public:
  enum class Key {
    Power,
    VolumeUp,
    VolumeDown,
  };

  Keys() {}
  virtual ~Keys() {}

  virtual int fd(const Key& key) = 0;

  virtual bool monitor(const Key& key, bool enable) = 0;
};

#endif /* KEYS_H */
