#include "asio.h"
#include <iostream>

class LoopIntegrationAsio::Service {
public:
  Service(uint64_t id) :
    m_id(id) {}

  virtual ~Service() {}

  uint64_t id() const { return m_id; }

  virtual bool start() = 0;
  virtual void stop() = 0;

private:
  const uint64_t m_id;
};

class WatchService : public LoopIntegrationAsio::Service {
public:
  WatchService(int fd, uint64_t id,
	       boost::asio::io_service& service,
	       const std::function<void(bool)>& cb) :
    Service(id),
    m_fd(fd),
    m_desc(service),
    m_cb(cb) {

  }

  ~WatchService() { stop(); }
  bool start() override;
  void stop() override { m_desc.cancel(); }

private:
  int m_fd;
  boost::asio::posix::stream_descriptor m_desc;
  std::function<void(bool)> m_cb;
};

class TimerService : public LoopIntegrationAsio::Service {
public:
  TimerService(int ms, uint64_t id,
	       boost::asio::io_service& service,
	       const std::function<void()>& cb) :
    Service(id),
    m_fd(-1),
    m_ms(ms),
    m_cb(cb),
    m_desc(service) {

  }

  ~TimerService() { stop(); }
  bool start() override;
  void stop() override { m_desc.cancel(); close(m_fd); m_fd = -1; }

private:
  int m_fd;
  int m_ms;
  std::function<void()> m_cb;
  boost::asio::posix::stream_descriptor m_desc;
};

bool WatchService::start() {
  if (m_desc.native_handle() != m_fd) {
    try {
      m_desc.assign(m_fd);
    } catch (...) {
      return false;
    }
  }

  auto func = [this](const boost::system::error_code& code, size_t) {
    if (code == boost::system::errc::operation_canceled) {
      // We should do nothing here
      // We can still be called by asio after the object gets destroyed
      return;
    }

    if (code) {
      // We have an error.
      m_cb(false);
    } else {
      m_cb(true);
      start();
    }
  };

  m_desc.async_read_some(boost::asio::null_buffers(), func);

  return true;
}

bool TimerService::start() {
  if (m_fd == -1) {
    m_fd = timerfd_create(CLOCK_MONOTONIC, 0);
    if (m_fd == -1) {
      return false;
    }

    struct itimerspec itval = {{0, m_ms * 1000000}, {0, m_ms * 1000000}};

    if (timerfd_settime(m_fd, 0, &itval, NULL) != 0) {
      close(m_fd);
      m_fd = -1;
      return false;
    }

    try {
      m_desc.assign(m_fd);
    } catch (...) {
      close(m_fd);
      m_fd = -1;
      return false;
    }
  }

  auto func = [this](const boost::system::error_code& code, size_t) {
    if (code == boost::system::errc::operation_canceled) {
      // We should do nothing here
      // We can still be called by asio after the object gets destroyed
      return;
    }

    if (!code) {
      uint64_t n = 0;
      read(m_fd, &n, sizeof(n));
      m_cb();
      start();
    }
  };

  m_desc.async_read_some(boost::asio::null_buffers(), func);

  return true;
}

LoopIntegrationAsio::LoopIntegrationAsio(boost::asio::io_service& service) :
  m_nextId(1),
  m_service(service) {

}

LoopIntegrationAsio::~LoopIntegrationAsio() {
  while (!m_services.empty()) {
    auto iter = m_services.begin();
    auto service = *iter;
    m_services.erase(iter);
    service->stop();
    delete service;
  }
}

uint64_t LoopIntegrationAsio::addFileDescriptor(int fd, const std::function<void(bool)>& cb) {
  Service *service = new WatchService(fd, m_nextId++, m_service, cb);
  return addService(service);
}

uint64_t LoopIntegrationAsio::post(int ms, const std::function<void()>& cb) {
  Service *service = new TimerService(ms, m_nextId++, m_service, cb);
  return addService(service);
}

void LoopIntegrationAsio::cancel(uint64_t id) {
  if (id <= 0) {
    return;
  }

  auto iter = std::find_if(m_services.begin(), m_services.end(), [id](Service *service) {
      return service->id() == id;
    });

  if (iter != m_services.end()) {
    auto service = *iter;
    m_services.erase(iter);
    service->stop();
    delete service;
  }
}

uint64_t LoopIntegrationAsio::addService(LoopIntegrationAsio::Service *service) {
  if (!service->start()) {
    --m_nextId;
    delete service;
    return 0;
  }

  m_services.push_back(service);
  return service->id();
}
