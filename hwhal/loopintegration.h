#ifndef LOOP_INTEGRATION_H
#define LOOP_INTEGRATION_H

#include <functional>
#include <cstdint>
#include <functional>
#include <memory>

class LoopIntegration {
public:
  class FdWatcher {
  public:
    virtual ~FdWatcher() {}
    virtual void stop() = 0;
  };

  class Timer {
  public:
    virtual ~Timer() {}
    virtual void stop() = 0;
  };

  virtual ~LoopIntegration() {}

  // watch an fd
  virtual std::unique_ptr<FdWatcher>
    addFileDescriptor(const std::function<void(bool)>& cb, int fd) = 0;

  // call me later
  virtual std::unique_ptr<Timer> post(const std::function<void()>& cb, int ms) = 0;
};

#endif /* LOOP_INTEGRATION_H */
