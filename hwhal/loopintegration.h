#ifndef LOOP_INTEGRATION_H
#define LOOP_INTEGRATION_H

#include <functional>
#include <cstdint>

class LoopIntegration {
public:
  LoopIntegration() {}
  virtual ~LoopIntegration() {}

  // The callback is of the form void callback(bool ok);
  // If ok is false then the callback will be removed after the callback returns.
  virtual uint64_t addFileDescriptor(int fd, const std::function<void(bool)>& cb) = 0;

  // remove id
  virtual void removeFileDescriptor(uint64_t id) = 0;

  // call cb later
  virtual uint64_t post(const std::function<void()>& cb, int ms = 1000) = 0;

  // prevent a post from running
  virtual void clear(uint64_t id);
};

#endif /* LOOP_INTEGRATION_H */
