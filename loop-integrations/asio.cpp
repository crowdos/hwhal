#include "asio.h"
#include <iostream>
class LoopIntegrationAsio::TimerService {
public:
  TimerService(boost::asio::io_service& service, const std::function<void()>& cb, int ms) :
    m_cb(cb),
    m_timer(service, boost::posix_time::milliseconds(ms)) {

    m_timer.async_wait([this](const boost::system::error_code& code){
	if (!code) {
	  m_cb();
	  delete this;
	}
      });
  }

  ~TimerService() {
    cancel();
    m_notify();
  }

  void cancel() {
    m_timer.cancel();
  }

  void setNotify(const std::function<void()>& notify) {
    m_notify = notify;
  }

private:
  std::function<void()> m_cb;
  boost::asio::deadline_timer m_timer;
  std::function<void()> m_notify;
};

class LoopIntegrationAsio::FdService {
public:
  FdService(boost::asio::io_service& service, const std::function<void(bool)>& cb, int fd) :
    m_desc(service),
    m_cb(cb) {
    m_desc.assign(fd);

    read();
  }

  ~FdService() {
    cancel();
    m_notify();
  }

  void cancel() {
    m_desc.cancel();
  }

  void setNotify(const std::function<void()>& notify) {
    m_notify = notify;
  }

private:
  void read() {
    auto func = [this](const boost::system::error_code& code, int) {
      if (code) {
	// We have an error.
	m_cb(false);
	delete this;
      } else {
	m_cb(true);
	read();
      }
    };

    m_desc.async_read_some(boost::asio::null_buffers(), func);
  }

  boost::asio::posix::stream_descriptor m_desc;
  std::function<void(bool)> m_cb;
  std::function<void()> m_notify;
};

class LoopIntegrationAsioFdWatcher : public LoopIntegrationAsio::FdWatcher {
public:
  LoopIntegrationAsioFdWatcher(LoopIntegrationAsio::FdService *service) :
    m_service(service) { }

  ~LoopIntegrationAsioFdWatcher() {
    delete m_service;
    m_service = nullptr;
  }

  void stop() {
    m_service->cancel();
  }

private:
  LoopIntegrationAsio::FdService *m_service;
};

class LoopIntegrationAsioTimer : public LoopIntegration::Timer {
public:
  LoopIntegrationAsioTimer(LoopIntegrationAsio::TimerService *service) :
    m_service(service) { }

  ~LoopIntegrationAsioTimer() {
    delete m_service;
    m_service = nullptr;
  }

  void stop() {
    m_service->cancel();
  }

private:
  LoopIntegrationAsio::TimerService *m_service;
};

LoopIntegrationAsio::LoopIntegrationAsio(boost::asio::io_service& service) :
  m_service(service) {

}

LoopIntegrationAsio::~LoopIntegrationAsio() {
  while (!m_fds.empty()) {
    auto iter = m_fds.begin();
    auto service = *iter;
    m_fds.erase(iter);
    delete service;
  }

  while (!m_timers.empty()) {
    auto iter = m_timers.begin();
    auto service = *iter;
    m_timers.erase(iter);
    delete service;
  }
}

std::unique_ptr<LoopIntegrationAsio::FdWatcher>
LoopIntegrationAsio::addFileDescriptor(const std::function<void(bool)>& cb, int fd) {
  LoopIntegrationAsio::FdService *service = new LoopIntegrationAsio::FdService(m_service, cb, fd);

  m_fds.push_back(service);

  service->setNotify([this, &service]() {
      auto iter =
	std::find_if(m_fds.begin(), m_fds.end(), [&service] (LoopIntegrationAsio::FdService *s){
	    std::cerr << "Delete fd " << s << " " << service;
	    return s == service;
	  });
      if (iter != m_fds.end()) {
	m_fds.erase(iter);
      }
    });

  LoopIntegration::FdWatcher *watcher = new LoopIntegrationAsioFdWatcher(service);

  return std::unique_ptr<LoopIntegration::FdWatcher>(watcher);
}

std::unique_ptr<LoopIntegrationAsio::Timer>
LoopIntegrationAsio::post(const std::function<void()>& cb, int ms) {
  LoopIntegrationAsio::TimerService *service =
    new LoopIntegrationAsio::TimerService(m_service, cb, ms);

  m_timers.push_back(service);

  service->setNotify([this, &service]() {
      auto iter =
	std::find_if(m_timers.begin(), m_timers.end(), [&service] (LoopIntegrationAsio::TimerService *s){
	    std::cerr << "Delete timer " << s << " " << service;
	    return s == service;
	  });
      if (iter != m_timers.end()) {
	m_timers.erase(iter);
      }
    });

  LoopIntegrationAsio::Timer *watcher = new LoopIntegrationAsioTimer(service);

  return std::unique_ptr<LoopIntegration::Timer>(watcher);
}
