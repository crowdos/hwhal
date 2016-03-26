#include "lightshal.h"

LightsHal::LightsHal() :
  m_brightness(10) {

}

int LightsHal::minBacklightBrightness() {
  return 0;
}

int LightsHal::maxBacklightBrightness() {
  return 100;
}

int LightsHal::backlightBrightness() {
  return m_brightness;
}

void LightsHal::setBacklightBrightness(int brightness) {
  m_brightness = brightness;
}
