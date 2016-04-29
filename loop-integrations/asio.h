#ifndef LOOP_INTEGRATION_ASIO_H
#define LOOP_INTEGRATION_ASIO_H

#include <hwhal/loopintegration.h>
#include <boost/asio.hpp>
#include <deque>

class LoopIntegrationAsio : public LoopIntegration {
public:
  class TimerService;
  class FdService;

  LoopIntegrationAsio(boost::asio::io_service& service);
  ~LoopIntegrationAsio();

  std::unique_ptr<FdWatcher> addFileDescriptor(const std::function<void(bool)>& cb, int fd);
  std::unique_ptr<Timer> post(const std::function<void()>& cb, int ms);

private:
  boost::asio::io_service& m_service;
  std::deque<TimerService *> m_timers;
  std::deque<FdService *> m_fds;
};

#endif /* LOOP_INTEGRATION_ASIO_H */
