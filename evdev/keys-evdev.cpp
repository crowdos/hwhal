#include "keys-evdev.h"
#include <list>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <hwhal/loopintegration.h>

// I hate evdev
#define OFF(x)  ((x)%BITS_PER_LONG)
#define LONG(x) ((x)/BITS_PER_LONG)
#define BITS_PER_LONG (sizeof(long) * 8)
#define NBITS(x) ((((x)-1)/BITS_PER_LONG)+1)
#define test_bit(bit, array)    ((array[LONG(bit)] >> OFF(bit)) & 1)

class HwHalKeysEvdevState {
public:
  HwHalKeysEvdevState(int code, LoopIntegration *loop, const std::function<void(bool)>& listener) :
    m_loop(loop),
    m_listener(listener),
    m_code(code),
    m_fd(-1),
    m_id(0) {

  }

  ~HwHalKeysEvdevState() {
    stop();
  }

  bool start() {
    if (m_fd != -1) {
      return true;
    }

    m_fd = findDevice(m_code);
    if (m_fd == -1) {
      return false;
    }

    // In the beginning, We semd the current state:
    unsigned long bits[NBITS(KEY_MAX)];
    memset(bits, 0, sizeof(bits));
    if (ioctl(m_fd, EVIOCGKEY(sizeof(bits)), bits) != -1) {
      m_listener(test_bit(m_code, bits) ? true : false);
    }

    m_id = m_loop->addFileDescriptor(m_fd, [this](bool ok) {
	// TODO: What should we do if ok is false?
	if (ok) {
	  struct input_event ev[64];
	  ssize_t s = read(m_fd, ev, sizeof(ev));
	  if (s < sizeof(struct input_event)) {
	    // TODO: What should we do here really?
	    return;
	  }

	  for (int i = 0; i < s / sizeof(struct input_event); i++) {
	    unsigned int type, code;
	    type = ev[i].type;
	    code = ev[i].code;
	    if (type == EV_KEY && code == m_code) {
	      m_listener(ev[i].value == 0 ? false : true);
	    }
	  }
	}
      });

    return true;
  }

  void stop() {
    if (m_id != 0) {
      m_loop->cancel(m_id);
      m_id = 0;
    }

    if (m_fd != -1) {
      close(m_fd);
      m_fd = -1;
    }
  }

private:
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


  LoopIntegration *m_loop;
  std::function<void(bool)> m_listener;
  int m_code;
  int m_fd;
  uint64_t m_id;
};

HwHalKeysEvdev::HwHalKeysEvdev(LoopIntegration *loop) :
  m_loop(loop) {

}

HwHalKeysEvdev::~HwHalKeysEvdev() {
  for (auto it : m_keys) {
    delete it.second;
  }

  m_keys.clear();
}

bool HwHalKeysEvdev::monitor(const Key& key, const std::function<void(bool)>& listener) {
  if (!listener) {
    auto iter = m_keys.find(key);
    if (iter != m_keys.end()) {
      HwHalKeysEvdevState *s = iter->second;
      m_keys.erase(iter);
      delete s;
    }

    return true;
  }

  HwHalKeysEvdevState *k = findKey(key);
  if (k) {
    return false;
  }

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

  k = new HwHalKeysEvdevState(code, m_loop, listener);
  if (!k->start()) {
    delete k;
    return false;
  }

  m_keys.insert(std::make_pair(key, k));

  return true;
}

HwHalKeysEvdevState *HwHalKeysEvdev::findKey(const Key& key) {
  auto it = m_keys.find(key);
  if (it == m_keys.end()) {
    return nullptr;
  }

  return m_keys[key];
}
