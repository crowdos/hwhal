#ifndef LOOP_INTEGRATION_GLIB_H
#define LOOP_INTEGRATION_GLIB_H

#include <hwhal/loopintegration.h>
#include <glib.h>
#include <deque>

class LoopIntegrationGlib : public LoopIntegration {
public:
  class Service;

  LoopIntegrationGlib();
  ~LoopIntegrationGlib();

  uint64_t addFileDescriptor(int fd, const std::function<void(bool)>& cb);
  uint64_t post(int ms, const std::function<void()>& cb);
  void cancel(uint64_t id);

private:
  uint64_t m_nextId;
  std::deque<Service *> m_services;
};

#endif /* LOOP_INTEGRATION_GLIB_H */
