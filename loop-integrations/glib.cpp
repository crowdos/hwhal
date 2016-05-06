#include "glib.h"
#include <glib.h>
#include <algorithm>

class LoopIntegrationGlib::Service {
public:
  Service(uint64_t id) :
    m_sourceId(0),
    m_id(id) {}

  virtual ~Service() { stop(); }

  uint64_t id() const { return m_id; }

  virtual void start() = 0;

  void stop() {
    if (m_sourceId > 0) {
      g_source_remove(m_sourceId);
      m_sourceId = 0;
    }
  }

protected:
  guint m_sourceId;

private:
  const uint64_t m_id;
};

class WatchService : public LoopIntegrationGlib::Service {
public:
  WatchService(int fd, uint64_t id,
	       const std::function<void(bool)>& cb) :
    Service(id),
    m_fd(fd),
    m_cb(cb) {

  }

  ~WatchService() { }
  void start();

private:
  static gboolean __WatchService_dispatch(G_GNUC_UNUSED GIOChannel *source,
					  GIOCondition condition,
					  WatchService *that) {
    bool error = (condition == G_IO_ERR ||
		  condition == G_IO_HUP ||
		  condition == G_IO_NVAL);
    that->m_cb(!error);

    if (error) {
      that->m_sourceId = 0;
      return FALSE;
    }

    return TRUE;
  }

  int m_fd;
  std::function<void(bool)> m_cb;
};

class TimerService : public LoopIntegrationGlib::Service {
public:
  TimerService(int ms, uint64_t id,
	       const std::function<void()>& cb) :
    Service(id),
    m_ms(ms),
    m_cb(cb) {

  }

  ~TimerService() { }
  void start();

private:
  static gboolean __TimerService_dispatch(TimerService *that) {
    that->m_cb();
    return TRUE;
  }

  int m_ms;
  std::function<void()> m_cb;
};

void WatchService::start() {
  GIOChannel *channel = g_io_channel_unix_new(m_fd);
  m_sourceId = g_io_add_watch(channel,
			      (GIOCondition)(G_IO_IN | G_IO_PRI | G_IO_ERR | G_IO_HUP | G_IO_NVAL),
			      (GIOFunc)__WatchService_dispatch,
			      this);
  g_io_channel_unref(channel);
}

void TimerService::start() {
  m_sourceId = g_timeout_add(m_ms, (GSourceFunc)__TimerService_dispatch, this);
}

LoopIntegrationGlib::LoopIntegrationGlib() :
  m_nextId(1)
{

}

LoopIntegrationGlib::~LoopIntegrationGlib() {

}

uint64_t LoopIntegrationGlib::addFileDescriptor(int fd, const std::function<void(bool)>& cb) {
  Service *service = new WatchService(fd, m_nextId++, cb);
  service->start();
  m_services.push_back(service);

  return service->id();
}

uint64_t LoopIntegrationGlib::post(int ms, const std::function<void()>& cb) {
  Service *service = new TimerService(ms, m_nextId++, cb);
  service->start();
  m_services.push_back(service);

  return service->id();
}

void LoopIntegrationGlib::cancel(uint64_t id) {
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
