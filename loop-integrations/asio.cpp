#include "asio.h"

class LoopIntegrationAsio::Service {
public:
  Service(uint64_t id) :
    m_id(id) {}

  virtual ~Service() {}

  uint64_t id() const { return m_id; }

  virtual void start() = 0;
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
    m_desc(service),
    m_cb(cb) {
    m_desc.assign(fd);
  }

  ~WatchService() { stop(); }
  void start();
  void stop() { m_desc.cancel(); }

private:
  boost::asio::posix::stream_descriptor m_desc;
  std::function<void(bool)> m_cb;
};

class TimerService : public LoopIntegrationAsio::Service {
public:
  TimerService(int ms, uint64_t id,
	       boost::asio::io_service& service,
	       const std::function<void()>& cb) :
    Service(id),
    m_timer(service, boost::posix_time::milliseconds(ms)),
    m_cb(cb) { }

  ~TimerService() { stop(); }
  void start();
  void stop() { m_timer.cancel(); }

private:
  std::function<void()> m_cb;
  boost::asio::deadline_timer m_timer;
};

void WatchService::start() {
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
}

void TimerService::start() {
  m_timer.async_wait([this](const boost::system::error_code& code){
      if (!code) {
	m_cb();
      }
    });
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
  service->start();
  m_services.push_back(service);

  return service->id();
}

uint64_t LoopIntegrationAsio::post(int ms, const std::function<void()>& cb) {
  Service *service = new TimerService(ms, m_nextId++, m_service, cb);
  service->start();
  m_services.push_back(service);

  return service->id();
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
