#ifndef LOOP_INTEGRATION_ASIO_H
#define LOOP_INTEGRATION_ASIO_H

#include <hwhal/loopintegration.h>
#include <boost/asio.hpp>
#include <deque>

class LoopIntegrationAsio : public LoopIntegration {
public:
  class Service;

  LoopIntegrationAsio(boost::asio::io_service& service);
  ~LoopIntegrationAsio();

  uint64_t addFileDescriptor(int fd, const std::function<void(bool)>& cb);
  uint64_t post(int ms, const std::function<void()>& cb);
  void cancel(uint64_t id);

private:
  uint64_t m_nextId;
  boost::asio::io_service& m_service;
  std::deque<Service *> m_services;
};

#endif /* LOOP_INTEGRATION_ASIO_H */
