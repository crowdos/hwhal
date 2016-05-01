#ifndef LOOP_INTEGRATION_H
#define LOOP_INTEGRATION_H

#include <cstdint>
#include <functional>

class LoopIntegration {
public:
  virtual ~LoopIntegration() {}

  // watch an fd
  virtual uint64_t addFileDescriptor(int fd, const std::function<void(bool)>& cb) = 0;

  // schedule a timer
  virtual uint64_t post(int ms, const std::function<void()>& cb) = 0;

  // cancel a watch ir a timer
  virtual void cancel(uint64_t id) = 0;
};

#endif /* LOOP_INTEGRATION_H */
