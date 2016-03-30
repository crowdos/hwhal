#include "keys-evdev.h"
#include <algorithm>
#include <list>
#include <thread>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>

class HwHalKeysEvdevState {
public:
  HwHalKeysEvdevState(int code) {
    m_fd = findDevice(code);
    if (m_fd == -1) {
      throw std::runtime_error("Cannot find evdev device");
    }

    if (socketpair(AF_UNIX, SOCK_STREAM | SOCK_NONBLOCK, 0, m_sv) == -1) {
      throw std::runtime_error("Failed to create sockets");
    }
  }

  ~HwHalKeysEvdevState() {
    stop();

    close(m_fd);
    m_fd = -1;

    close(m_sv[0]);
    m_sv[0] = -1;

    close(m_sv[1]);
    m_sv[1] = -1;
  }

  bool start() {
    if (m_thread.joinable()) {
      // Already running
      return true;
    }

    m_thread = std::thread(&HwHalKeysEvdevState::run, this);
  }

  bool stop() {
    pthread_cancel(m_thread.native_handle());
    try {
      m_thread.join();
    } catch (...) {
      // Nothing
    }

    return true;
  }

  int fd() {
    return m_sv[1];
  }

private:
  void run() {
    struct input_event ev[64];
    while (true) {
      ssize_t s = read(m_fd, ev, sizeof(ev));

      if (s < sizeof(struct input_event)) {
	// TODO: What should we do here really?
	continue;
      }

      for (int i = 0; i < s / sizeof(struct input_event); i++) {
	unsigned int type, code;
	type = ev[i].type;
	code = ev[i].code;
	if (type == EV_KEY && code == m_code) {
	  char c = ev[i].value == 0 ? KEY_RELEASED : KEY_PRESSED;
	  write(m_sv[0], &c, sizeof(c));
	}
      }
    }
  }

  int findDevice(int code) {
    std::list<std::string> devs;

    struct dirent **namelist;
    int n = scandir("/dev/input/", &namelist, [](const struct dirent *d) {
	std::string p(d->d_name);
	return p.size() > 5 && p.substr(0, 5) == "event" ? 1 : 0;
      },
      alphasort);

    if (n < 0) {
      return -1;
    }

    while (n--) {
      devs.push_back(std::string("/dev/input/") + namelist[n]->d_name);
      free(namelist[n]);
    }

    free(namelist);

    for (const auto it : devs) {
      m_fd = open(it.c_str(), O_RDONLY);
      if (m_fd == -1) {
	continue;
      }

      if (has_code(m_fd, code)) {
	// We are fine
	m_code = code;
	return m_fd;
      }

      close(m_fd);
      m_fd = -1;
    }

    return -1;
  }

  bool has_code(int fd, int code) {
    // I hate evdev
#define OFF(x)  ((x)%BITS_PER_LONG)
#define LONG(x) ((x)/BITS_PER_LONG)
#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define test_bit(bit, array)    ((array[LONG(bit)] >> OFF(bit)) & 1)

    unsigned long bit[EV_MAX][NBITS(KEY_MAX)];
    memset(bit, 0, sizeof(bit));
    if (ioctl(fd, EVIOCGBIT(0, EV_MAX), bit[0]) == -1) {
      return false;
    }

    if (ioctl(fd, EVIOCGBIT(EV_KEY, KEY_MAX), bit[EV_KEY]) == -1) {
      return false;
    }

    return test_bit(EV_SYN, bit[0]) && test_bit(EV_KEY, bit[0]) && test_bit(code, bit[EV_KEY]);
  }

  std::thread m_thread;
  int m_code = -1;
  int m_fd = -1;
  int m_sv[2] = {-1, -1};
};

HwHalKeysEvdev::HwHalKeysEvdev() {

}

HwHalKeysEvdev::~HwHalKeysEvdev() {
  for (auto it : m_keys) {
    delete it.second;
  }

  m_keys.clear();
}

int HwHalKeysEvdev::fd(const Key& key) {
  HwHalKeysEvdevState *k = findKey(key);
  if (!k) {
    int code = -1;

    switch (key) {
    case Keys::Key::Power:
      code = KEY_POWER;
      break;

    case Keys::Key::VolumeUp:
      code = KEY_VOLUMEUP;
      break;

    case Keys::Key::VolumeDown:
      code = KEY_VOLUMEDOWN;
      break;
    }

    try {
      k = new HwHalKeysEvdevState(code);
      m_keys.insert(std::make_pair(key, k));
    } catch (...) {
      return -1;
    }
  }

  return k->fd();
}

bool HwHalKeysEvdev::monitor(const Key& key, bool enable) {
  HwHalKeysEvdevState *k = findKey(key);
  if (!k) {
    return false;
  }

  if (enable) {
    return k->start();
  } else {
    return k->stop();
  }
}

HwHalKeysEvdevState *HwHalKeysEvdev::findKey(const Key& key) {
  auto it = m_keys.find(key);
  if (it == m_keys.end()) {
    return nullptr;
  }

  return m_keys[key];
}
